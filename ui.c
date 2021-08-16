#include <stdio.h>
#include <string.h>

#include "ui.h"

/* Constants */
static const char *const WifiIcon = "";
static const char *const MemoryIcon = "";
static const char *const DiskIcon = "";
static const char *const CPUIcon = "";
static const char *const SpeakerIconUnmuted = "";
static const char *const SpeakerIconMuted = "";
static const char *const DateIcon = "";
static const char *const TimeIcon = "";
static const char *const TimeIconWithSpace = " ";
static const char *const BatteryIconCharging = "";
static const char *const BatteryIcon100 = "";
static const char *const BatteryIcon75 = "";
static const char *const BatteryIcon50 = "";
static const char *const BatteryIcon25 = "";
static const char *const BatteryIcon0 = "";
static const char *const TimeFormat = "%R";
static const char *const DateFormat = "%A %x";
static const char *const CyanFG = "^c#75b7bf^";
static const char *const LightCyanFG = "^c#88c9d1^";
static const char *const MagentaFG = "^c#ba5d89^";
static const char *const LightMagentaFG = "^c#cc6c99^";
static const char *const YellowFG = "^c#edbf53^";
static const char *const LightYellowFG = "^c#f0c665^";
static const char *const GreenFG = "^c#48a374^";
static const char *const LightGreenFG = "^c#5aad82^";
static const char *const RedFG = "^c#f28a4a^";
static const char *const LightRedFG = "^c#eb8e54^";
static const char *const BlueFG = "^c#4e8cb7^";
static const char *const LightBlueFG = "^c#68a7d4^";
static const char *const WhiteFG = "^c#d1c9c9^";
static const char *const BlackFG = "^c#63697d^";
static const char *const PinkFG = "^c#f7a8b8^";
static const char *const OrangeFG = "^c#eb8e54^";
static const char *const SchemeReset = "^d^";
static const float Gigabyte = 1024 * 1024 * 1024;

void setOutput(dwmStatus *status)
{
    int volume, outputLength;
    float memory, disk, cpu, wifi, battery;

    volume = status->volume.percent + 0.5;
    memory = (float)status->memory.usedBytes / status->memory.totalBytes * 100;
    disk = (float)status->disk.usedBytes / status->disk.totalBytes * 100;
    cpu = status->cpu.utilization;
    wifi = status->wifi.strength;
    battery = status->battery.percent;

    status->output[0] = 0;

    if (status->wifi.active) {
        outputLength = strlen(status->output);
        snprintf(status->output + outputLength,
                 MAX_STATUS_OUTPUT - outputLength,
                 " %s%s%s %.1f%% ",
                 LightBlueFG, status->wifi.icon, SchemeReset, wifi);
    } else if (status->wifi.capable) {
        outputLength = strlen(status->output);
        snprintf(status->output + outputLength,
                 MAX_STATUS_OUTPUT - outputLength,
                 "           ",
                 LightBlueFG, status->wifi.icon, SchemeReset);
    }

    outputLength = strlen(status->output);
    snprintf(status->output + outputLength, MAX_STATUS_OUTPUT - outputLength,
             " %s%s%s %.1f%%  %s%s%s %.0f%%  %s%s%s %.0f%%  %s%s%s %d%%  ",
             LightBlueFG, status->cpu.icon, SchemeReset, cpu,
             LightBlueFG, status->memory.icon, SchemeReset, memory,
             LightBlueFG, status->disk.icon, SchemeReset, disk,
             LightBlueFG, status->volume.icon, SchemeReset, volume);

    if (status->battery.active) {
        outputLength = strlen(status->output);
        snprintf(status->output + outputLength,
                 MAX_STATUS_OUTPUT - outputLength,
                 "%s%s%s %.0f%%  ",
                 LightBlueFG, status->battery.icon, SchemeReset, battery);
    }

    outputLength = strlen(status->output);
    snprintf(status->output + outputLength, MAX_STATUS_OUTPUT - outputLength,
             "%s%s%s %s  %s%s%s%s      ",
             LightBlueFG, status->date.icon, SchemeReset, status->date.output,
             LightBlueFG, status->time.icon, SchemeReset, status->time.output);
}

void setDateFormat(dwmDate *date)
{
    date->format = DateFormat;
}

void setTimeFormat(dwmTime *time)
{
    time->format = TimeFormat;
}

void setBatteryIcon(dwmBattery *battery)
{
    if (battery->charging) {
        battery->icon = BatteryIconCharging;
    } else {
        switch ((int)battery->percent) {
        case 90 ... 100:
            battery->icon = BatteryIcon100;
            break;
        case 60 ... 89:
            battery->icon = BatteryIcon75;
            break;
        case 30 ... 59:
            battery->icon = BatteryIcon50;
            break;
        case 10 ... 29:
            battery->icon = BatteryIcon25;
            break;
        default:
            battery->icon = BatteryIcon0;
            break;
        }
    }
}

void setCPUIcon(dwmCPU *cpu)
{
    cpu->icon = CPUIcon;
}

/*
 * Provides consistent spacing between the time icon and time value
 * for both single and double digit hours throughout the day.
 * All double digit hours are prefixed with an empty space.
 */
void setTimeIcon(dwmTime *time)
{
    time->icon = time->output[0] != ' ' ? TimeIconWithSpace : TimeIcon;
}

void setDateIcon(dwmDate *date)
{
    date->icon = DateIcon;
}

void setDiskIcon(dwmDisk *disk)
{
    disk->icon = DiskIcon;
}

void setMemoryIcon(dwmMemory *memory)
{
    memory->icon = MemoryIcon;
}

void setVolumeIcon(dwmVolume *volume)
{
    volume->icon = volume->muted ? SpeakerIconUnmuted : SpeakerIconMuted;
}

void setWifiIcon(dwmWifi *wifi)
{
    wifi->icon = WifiIcon;
}
