#include <stdio.h>
#include <string.h>
#include <sys/statvfs.h>
#include "hardware.h"

void get_cpu_info(CpuInfo *info){

FILE *file = fopen("/proc/cpuinfo", "r");
char line[256];

strcpy (info ->model_name, "unknown");
info->cores = 0;

if (file == NULL) {
        printf("[ERROR] can't read /proc/cpuinfo\n");
        return;
    }



while (fgets(line, sizeof(line), file)) {

if (strncmp(line, "model name", 10) == 0) {
            char *colon = strchr(line, ':'); 
            if (colon != NULL) {
                
                sscanf(colon + 2, "%[^\n]", info->model_name);
            }
        }
       
        if (strncmp(line, "cpu cores", 9) == 0) {
            char *colon = strchr(line, ':');
            if (colon != NULL) {
              
                sscanf(colon + 2, "%d", &info->cores);
            }
        }
    }

  
    fclose(file);
}

void get_ram_metrics(SystemMetrics *metrics) {
    FILE *file = fopen("/proc/meminfo", "r");
    char line[256];

    metrics->ram_total_kb = 0;
    metrics->ram_available_kb = 0;

    if (file == NULL) {
        printf("[ERREUR] Impossible de lire /proc/meminfo\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "MemTotal:", 9) == 0) {
            sscanf(line, "MemTotal: %ld kB", &metrics->ram_total_kb);
        }
        if (strncmp(line, "MemAvailable:", 13) == 0) {
            sscanf(line, "MemAvailable: %ld kB", &metrics->ram_available_kb);
        }
    }

    fclose(file);

    if (metrics->ram_total_kb > 0) {
        metrics->ram_used_kb = metrics->ram_total_kb - metrics->ram_available_kb;
        metrics->ram_usage_percent = (int)((metrics->ram_used_kb * 100.0) / metrics->ram_total_kb);
    } else {
        metrics->ram_used_kb = 0;
        metrics->ram_usage_percent = 0;
    }
}

void get_cpu_temperature(SystemMetrics *metrics) {
    
    FILE *file = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    
    metrics->cpu_temp_c = 0.0;

    if (file == NULL) {
        
        return; 
    }

    long temp_millidegrees;
    if (fscanf(file, "%ld", &temp_millidegrees) == 1) {
       
        metrics->cpu_temp_c = temp_millidegrees / 1000.0;
    }

    
    fclose(file);
}

void get_cpu_usage(SystemMetrics *metrics) {
    FILE *file = fopen("/proc/stat", "r");
    char line[256];
    
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

    if (file == NULL) return;

    if (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "cpu ", 4) == 0) {
            sscanf(line, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu",
                   &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);

            unsigned long long current_idle = idle + iowait;
            unsigned long long current_non_idle = user + nice + system + irq + softirq + steal;
            unsigned long long current_total = current_idle + current_non_idle;

            if (metrics->prev_total != 0) {
                unsigned long long total_diff = current_total - metrics->prev_total;
                unsigned long long idle_diff = current_idle - metrics->prev_idle;
                
                if (total_diff > 0) {
                    metrics->cpu_usage_percent = (int)(((total_diff - idle_diff) * 100.0) / total_diff);
                }
            } else {
                metrics->cpu_usage_percent = 0; 
            }

            metrics->prev_idle = current_idle;
            metrics->prev_total = current_total;
        }
    }
    fclose(file); 
}

void get_storage_metrics(SystemMetrics *metrics) {
    struct statvfs stat;

    
    if (statvfs("/", &stat) != 0) {
      
        metrics->storage_total_gb = 0;
        metrics->storage_used_gb = 0;
        metrics->storage_usage_percent = 0;
        return;
    }

   
    unsigned long long total_bytes = stat.f_blocks * stat.f_frsize;
    unsigned long long free_bytes = stat.f_bfree * stat.f_frsize;
    unsigned long long used_bytes = total_bytes - free_bytes;

    
    metrics->storage_total_gb = total_bytes / (1024 * 1024 * 1024);
    metrics->storage_used_gb = used_bytes / (1024 * 1024 * 1024);

    
    if (metrics->storage_total_gb > 0) {
        metrics->storage_usage_percent = (int)((metrics->storage_used_gb * 100.0) / metrics->storage_total_gb);
    } else {
        metrics->storage_usage_percent = 0;
    }
      
}
   
