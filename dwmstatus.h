#ifndef DWMSTATUS_H
#define DWMSTATUS_H

#include <X11/Xlib.h>

/* Macros */
#define MAX_STATUS_OUTPUT 512
#define MAX_DATE_OUTPUT 32

/* Typedefs */
typedef struct {
    int active;
    int capable;
    float strength;
    const char *icon;
} dwmWifi;

typedef struct {
   int active;
   int charging;
   float percent;
   unsigned long currentWh;
   unsigned long capacityWh;
   const char *icon;
} dwmBattery;

typedef struct {
   float utilization;
   unsigned long idleJiffies;
   unsigned long loadJiffies;
   const char *icon;
} dwmCPU;

typedef struct {
    char output[MAX_DATE_OUTPUT];
    const char *format;
    const char *icon;
} dwmDate, dwmTime;

typedef struct {
    unsigned long usedBytes;
    unsigned long totalBytes;
    const char *icon;
} dwmDisk, dwmMemory;

typedef struct {
    float percent;
    long current;
    long min;
    long max;
    int muted;
    const char *icon;
} dwmVolume;

typedef struct {
   Display *display;
   dwmBattery battery;
   dwmMemory memory;
   dwmVolume volume;
   dwmDate date;
   dwmTime time;
   dwmDisk disk;
   dwmWifi wifi;
   dwmCPU cpu;
   char output[MAX_STATUS_OUTPUT];
} dwmStatus;

/* Function prototypes */
void setBattery(dwmBattery *battery);
void setMemory(dwmMemory *memory);
void setVolume(dwmVolume *volume);
void setWifi(dwmWifi *wifi);
void setCPU(dwmCPU *cpu);
void setDisk(dwmDisk *disk);
void setDate(dwmDate *date);
void setTime(dwmTime *time);

#endif
