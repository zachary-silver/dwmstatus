/* Compile with -lX11 and -lasound flags */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <alsa/control.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

#include "dwmstatus.h"
#include "io.h"

/* Macros */
#define TRUE 1
#define FALSE 0
#define CPU_LOAD_INFO_COLUMNS 7
#define CPU_LOAD_IDLE_COLUMN 3
#define MAX_BUFFER_SIZE 1024
#define MAX_CHARGING_BUFFER 32
#define MAX_MEMORY_OUTPUT 8
#define MAX_VOLUME_OUTPUT 8
#define MAX_WIFI_OUTPUT 16
#define MAX_DISK_OUTPUT 16
#define MAX_CPU_OUTPUT 16
#define LENGTH_OF(x) (int)(sizeof(x) / sizeof((x)[0]))

/* Constants */
static const char *const WirelessInterface = "wlp4s0";
static const char *const CurrentBatteryFiles[] =
{
   "/sys/class/power_supply/BAT0/energy_now",
   "/sys/class/power_supply/BAT1/energy_now",
};
static const char *const CapacityBatteryFiles[] =
{
   "/sys/class/power_supply/BAT0/energy_full",
   "/sys/class/power_supply/BAT1/energy_full",
};
static const char *const StatusBatteryFiles[] =
{
   "/sys/class/power_supply/BAT0/status",
   "/sys/class/power_supply/BAT1/status",
};

/* Function prototypes */
unsigned long getBatteryValue(const char *fileName);
int batteryCharging(const char *fileName);
int getQualityLinkValue(char *wirelessInterfaceInfo);

/*
 * Equivalent to xsetroot -name $(status_output) in bash
 */
void output(dwmStatus *status)
{
    XStoreName(status->display, DefaultRootWindow(status->display),
               status->output);
    XSync(status->display, False);
}

/*
 * In conditions where values can't be read from files
 * and added to appropriate variables, battery active
 * will be set to false.
 */
void setBattery(dwmBattery *battery)
{
    int i, batteryCount = LENGTH_OF(CurrentBatteryFiles);

    battery->currentWh = 0;
    battery->capacityWh = 1;
    battery->charging = FALSE;

    for (i = 0; i < batteryCount; i++) {
        battery->currentWh += getBatteryValue(CurrentBatteryFiles[i]);
        battery->capacityWh += getBatteryValue(CapacityBatteryFiles[i]);
    }

    for (i = 0; i < batteryCount && !battery->charging; i++) {
        battery->charging = batteryCharging(StatusBatteryFiles[i]);
    }

    battery->active = battery->capacityWh > 1;
    battery->percent = ((double)battery->currentWh / battery->capacityWh) * 100;
}

unsigned long getBatteryValue(const char *fileName)
{
    unsigned long value = 0;
    FILE *fd;

    if ((fd = fopen(fileName, "r")) != NULL) {
        fscanf(fd, "%lu", &value);
        fclose(fd);
    }

    return value;
}

int batteryCharging(const char *fileName)
{
    char status[MAX_CHARGING_BUFFER];
    FILE *fd;

    if ((fd = fopen(fileName, "r")) != NULL) {
        fgets(status, MAX_CHARGING_BUFFER, fd);
        fclose(fd);
    }

    return strcmp(status, "Charging\n") == 0;
}

/*
 * Ignores the first column of /proc/stat's output which only
 * contains the string 'cpu' and reads the next 7 columns
 * which contains all the appropriate cpu load information needed
 * to calculate total cpu percent utilization.
 */
void setCPU(dwmCPU *cpu)
{
    unsigned long loadInfo[CPU_LOAD_INFO_COLUMNS],
                  loadJiffies, idleJiffies,
                  loadDelta, idleDelta;
    FILE *fd;
    int i;

    if ((fd = fopen("/proc/stat","r")) == NULL) {
        return;
    }
    fscanf(fd, "%*s %lu %lu %lu %lu %lu %lu %lu",
           &loadInfo[0], &loadInfo[1], &loadInfo[2], &loadInfo[3],
           &loadInfo[4], &loadInfo[5], &loadInfo[6]);
    fclose(fd);

    loadJiffies = 0;
    idleJiffies = loadInfo[CPU_LOAD_IDLE_COLUMN];
    for (i = 0; i < CPU_LOAD_INFO_COLUMNS; i++) {
        loadJiffies += loadInfo[i];
    }

    loadDelta = cpu->loadJiffies < loadJiffies ? loadJiffies - cpu->loadJiffies
                                               : cpu->loadJiffies - loadJiffies;
    idleDelta = cpu->idleJiffies < idleJiffies ? idleJiffies - cpu->idleJiffies
                                               : cpu->idleJiffies - idleJiffies;

    cpu->loadJiffies = loadJiffies;
    cpu->idleJiffies = idleJiffies;
    cpu->utilization = 100 * (loadDelta - idleDelta) / (float)loadDelta;
}

void setTime(dwmTime *time)
{
   setDate(time);
}

void setDate(dwmDate *date)
{
    time_t currentTime = time(NULL);

    strftime(date->output, MAX_DATE_OUTPUT,
             date->format, localtime(&currentTime));
}

void setDisk(dwmDisk *disk)
{
    struct statvfs diskInfo;

    if (statvfs("/", &diskInfo) < 0) {
        return;
    }

    disk->totalBytes = diskInfo.f_blocks * diskInfo.f_bsize;
    disk->usedBytes = disk->totalBytes - (diskInfo.f_bfree * diskInfo.f_bsize);
}

void setMemory(dwmMemory *memory)
{
    struct sysinfo sysInfo;

    if (sysinfo(&sysInfo) < 0) {
        return;
    }

    memory->usedBytes = (sysInfo.totalram - sysInfo.freeram);
    memory->totalBytes = sysInfo.totalram;
}

/*
 * For more detail and documentation on what's involved here visit:
 * https://www.alsa-project.org/alsa-doc/alsa-lib/group___simple_mixer.html
 */
void setVolume(dwmVolume *volume)
{
    static const snd_mixer_selem_channel_id_t channel = SND_MIXER_SCHN_MONO;
    static const char *soundCard = "default";
    static const char *channelName = "Master";
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t *elem;

    snd_mixer_open(&handle, 0);

    snd_mixer_attach(handle, soundCard);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, channelName);

    elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &volume->min, &volume->max);
    snd_mixer_selem_get_playback_volume(elem, 0, &volume->current);
    snd_mixer_selem_get_playback_switch(elem, channel, &volume->muted);

    snd_mixer_close(handle);

    volume->percent = ((double)volume->current / volume->max) * 100;
}

/*
 * Searches /proc/net/wireless for specified WirelessInterface's
 * quality link value. If result is null it's likely because there
 * is no active connection. The third column in the wireless interface's
 * output line indicates the end of the quality link value.
 */
void setWifi(dwmWifi *wifi)
{
    char wifiInfo[MAX_BUFFER_SIZE], *interfaceInfo;
    int fd, bytesRead;

    fd = openFile("/proc/net/wireless", 'r');
    bytesRead = read(fd, wifiInfo, MAX_BUFFER_SIZE - 1);
    closeFile(fd);

    if (bytesRead == -1) {
        return;
    }

    wifiInfo[bytesRead] = '\0';

    if ((interfaceInfo = strstr(wifiInfo, WirelessInterface)) == NULL) {
        wifi->active = FALSE;
        return;
    }

    wifi->active = TRUE;
    wifi->strength = getQualityLinkValue(interfaceInfo) * 100 / 70.0;
}

/*
 * Each call to strtok retrieves the data in the
 * next column pointed to by interfaceInfo.
 * The third column is the quality link value.
 */
int getQualityLinkValue(char *interfaceInfo)
{
    char *result;

    result = strtok(interfaceInfo, " .");
    result = strtok(NULL, " .");
    result = strtok(NULL, " .");

    return atoi(result);
}
