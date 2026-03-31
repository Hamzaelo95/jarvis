#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "hardware.h" 

#define SLEEP_INTERVAL 5

int main(int argc, char *argv[]) {
    printf("[JARVIS] Démarrage du Daemon Jarvis Boxtea...\n");

    CpuInfo my_cpu; 
    get_cpu_info(&my_cpu); 
    printf("[JARVIS] ---> Modèle CPU : %s (%d cœurs)\n", my_cpu.model_name, my_cpu.cores);

    SystemMetrics my_metrics = {0};

    printf("[JARVIS] Entrée dans la boucle de surveillance.\n\n");
    
    while (true) {
       
        get_ram_metrics(&my_metrics);
        get_cpu_temperature(&my_metrics);
        get_cpu_usage(&my_metrics);
        get_storage_metrics(&my_metrics);
        
     // B. Le Grand Affichage
        printf("[JARVIS] CPU: %d%% (%.1f °C) | RAM: %ld MB (%d%%) | Disque: %ld GB / %ld GB (%d%%)\n", 
               my_metrics.cpu_usage_percent,
               my_metrics.cpu_temp_c, 
               my_metrics.ram_used_kb / 1024, 
               my_metrics.ram_usage_percent,
               my_metrics.storage_used_gb,
               my_metrics.storage_total_gb,
               my_metrics.storage_usage_percent);

        sleep(SLEEP_INTERVAL);
    }

    return 0;
}