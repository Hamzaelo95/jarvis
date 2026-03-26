#ifndef HARDWARE_H
#define HARDWARE_H

typedef struct {
    char model_name[128];
    int cores;
} CpuInfo;

typedef struct {
    long ram_total_kb;
    long ram_available_kb;
    long ram_used_kb;
    int ram_usage_percent;
    float cpu_temp_c;
} SystemMetrics;

void get_cpu_info(CpuInfo *info);
void get_ram_metrics(SystemMetrics *metrics);
void get_cpu_temperature(SystemMetrics *metrics); 

#endif