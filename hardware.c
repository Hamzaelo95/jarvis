#include <stdio.h>
#include <string.h>
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