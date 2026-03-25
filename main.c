#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "hardware.h" 

#define SLEEP_INTERVAL 5 

int main(int argc, char *argv[]) {
    printf("[ALFRED] Démarrage du Daemon Alfred Boxtea...\n");

   
    printf("[ALFRED] Lecture de la configuration matérielle...\n");
    
   
    CpuInfo my_cpu; 
    
    
    get_cpu_info(&my_cpu); 
    
    // On affiche ce que la fonction a trouvé !
    printf("[ALFRED] ---> Modèle CPU : %s\n", my_cpu.model_name);
    printf("[ALFRED] ---> Nombre de cœurs : %d\n", my_cpu.cores);

    // 2. Boucle principale du Daemon (infinie)
    printf("[ALFRED] Entrée dans la boucle de surveillance.\n");
    
    while (true) {
        // A. Récupération des métriques dynamiques (à venir)
        // B. Construction du JSON et envoi à l'API Django (à venir)

        printf("[ALFRED] Tick... Attente de %d secondes...\n", SLEEP_INTERVAL);
        
        // C. Pause
        sleep(SLEEP_INTERVAL);
    }

    return 0;
}