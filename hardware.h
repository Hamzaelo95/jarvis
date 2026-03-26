#ifndef HARDWARE_H
#define HARDWARE_H


typedef struct {
    char model_name[128];
    int cores;
} CpuInfo;


void get_cpu_info(CpuInfo *info);

typedef struct {
    long ram_total_kb;
    long ram_available_kb;
    long ram_used_kb;
    int ram_usage_percent; 
} SystemMetrics;


void get_ram_metrics(SystemMetrics *metrics);
#endif