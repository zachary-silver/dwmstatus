#include <stdio.h>
#include <time.h>
#include <string.h>

#include "dwmstatus.h"
#include "gpmdp.h"

static const float Gigabyte = 1024 * 1024 * 1024;

void setOutput(dwmStatus *status);

int main(void)
{
    struct timespec sleepTime = { .tv_sec = 1, .tv_nsec = 000000000 };
    dwmStatus status = { 0 };
    gpmdpSong song;

    status.display = XOpenDisplay(NULL);

    setDateIcon(&status.date);
    setWifiIcon(&status.wifi);
    setDiskIcon(&status.disk);
    setMemoryIcon(&status.memory);
    setCPUIcon(&status.cpu);
    setDateIcon(&status.date);
    setDateFormat(&status.date);
    setTimeFormat(&status.time);

loop:
    setDate(&status.date);
    setTime(&status.time);
    setTimeIcon(&status.time);
    setBattery(&status.battery);
    setBatteryIcon(&status.battery);
    setVolume(&status.volume);
    setVolumeIcon(&status.volume);
    setMemory(&status.memory);
    setDisk(&status.disk);
    setCPU(&status.cpu);
    setWifi(&status.wifi);
    setSong(&song);
    setSongIcons(&song);
    setSongOutput(&song);
    snprintf(status.output, MAX_STATUS_OUTPUT, "%s  ", song.output);
    setOutput(&status);

    output(&status);

    nanosleep(&sleepTime, NULL);
    goto loop;
}

void setOutput(dwmStatus *status)
{
    int volume, usedMemory, totalMemory, usedDisk, totalDisk, outputLength;

    volume = status->volume.percent + 0.5;
    usedMemory = status->memory.usedBytes / Gigabyte + 0.5;
    totalMemory = status->memory.totalBytes / Gigabyte + 0.5;
    usedDisk = status->disk.usedBytes / Gigabyte + 0.5;
    totalDisk = status->disk.totalBytes / Gigabyte + 0.5;

    if (status->wifi.active) {
        outputLength = strlen(status->output);
        snprintf(status->output + outputLength,
                 MAX_STATUS_OUTPUT - outputLength,
                 "%s %.1f%%  ", status->wifi.icon, status->wifi.strength);
    }

    outputLength = strlen(status->output);
    snprintf(status->output + outputLength, MAX_STATUS_OUTPUT - outputLength,
             "%s %.1f%%  %s %d/%dG  %s %d/%dG  %s %d%%  ",
             status->cpu.icon, status->cpu.utilization,
             status->memory.icon, usedMemory, totalMemory,
             status->disk.icon, usedDisk, totalDisk,
             status->volume.icon, volume);

    if (status->battery.active) {
        outputLength = strlen(status->output);
        snprintf(status->output + outputLength,
                 MAX_STATUS_OUTPUT - outputLength, "%s %.0f%%  ",
                 status->battery.icon, status->battery.percent);
    }

    outputLength = strlen(status->output);
    snprintf(status->output + outputLength, MAX_STATUS_OUTPUT - outputLength,
             "%s %s  %s%s",
             status->date.icon, status->date.output,
             status->time.icon, status->time.output);
}
