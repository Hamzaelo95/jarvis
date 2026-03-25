#ifndef HARDWARE_H
#define HARDWARE_H


typedef struct {
    char model_name[128];
    int cores;
} CpuInfo;


void get_cpu_info(CpuInfo *info);

#endif