#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "hardware.h" 

#define SLEEP_INTERVAL 5

int main(int argc, char *argv[]) {
    printf("[ALFRED] Démarrage du Daemon Alfred Boxtea...\n");

    CpuInfo my_cpu; 
    get_cpu_info(&my_cpu); 
    printf("[ALFRED] ---> Modèle CPU : %s (%d cœurs)\n", my_cpu.model_name, my_cpu.cores);

    SystemMetrics my_metrics;

    printf("[ALFRED] Entrée dans la boucle de surveillance.\n\n");
    
    while (true) {
        get_ram_metrics(&my_metrics);
        
        printf("[ALFRED] RAM Utilisée : %ld MB / %ld MB (%d %%)\n", 
               my_metrics.ram_used_kb / 1024, 
               my_metrics.ram_total_kb / 1024, 
               my_metrics.ram_usage_percent);

        sleep(SLEEP_INTERVAL);
    }

    return 0;
}