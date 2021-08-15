#include <stdio.h>
#include <time.h>
#include <string.h>

#include "dwmstatus.h"
#include "ui.h"

int main(void)
{
    struct timespec sleepTime = { .tv_sec = 1, .tv_nsec = 000000000 };
    dwmStatus status = { 0 };

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

    setOutput(&status);

    output(&status);

    nanosleep(&sleepTime, NULL);
    goto loop;
}
