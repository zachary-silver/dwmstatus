#ifndef UI_H
#define UI_H

#include "dwmstatus.h"

/* Function prototypes */
void output(dwmStatus *status);
void setOutput(dwmStatus *status);
void setBatteryIcon(dwmBattery *battery);
void setMemoryIcon(dwmMemory *memory);
void setVolumeIcon(dwmVolume *volume);
void setWifiIcon(dwmWifi *wifi);
void setCPUIcon(dwmCPU *cpu);
void setDiskIcon(dwmDisk *disk);
void setDateIcon(dwmDate *date);
void setDateFormat(dwmDate *date);
void setTimeIcon(dwmTime *time);
void setTimeFormat(dwmTime *time);

#endif
