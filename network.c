#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <curl/curl.h>
#include "network.h"

void send_data_to_api(CpuInfo *cpu, SystemMetrics *metrics, const char *target, const char *api_key) {
    struct json_object *root = json_object_new_object();
    struct json_object *hardware = json_object_new_object();

    json_object_object_add(hardware, "cpu_model", json_object_new_string(cpu->model_name));
    json_object_object_add(hardware, "cpu_cores", json_object_new_int(cpu->cores));
    json_object_object_add(hardware, "cpu_usage_percent", json_object_new_int(metrics->cpu_usage_percent));
    json_object_object_add(hardware, "cpu_temp_c", json_object_new_double(metrics->cpu_temp_c));
    
    json_object_object_add(hardware, "ram_total_mb", json_object_new_int64(metrics->ram_total_kb / 1024));
    json_object_object_add(hardware, "ram_used_mb", json_object_new_int64(metrics->ram_used_kb / 1024));
    json_object_object_add(hardware, "ram_usage_percent", json_object_new_int(metrics->ram_usage_percent));
    
    json_object_object_add(hardware, "storage_total_gb", json_object_new_int64(metrics->storage_total_gb));
    json_object_object_add(hardware, "storage_used_gb", json_object_new_int64(metrics->storage_used_gb));
    json_object_object_add(hardware, "storage_usage_percent", json_object_new_int(metrics->storage_usage_percent));

    json_object_object_add(root, "HARDWARE", hardware);

    const char *json_string = json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY);
    printf("\n[JARVIS NETWORK] Préparation du JSON terminée.\n");

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        struct curl_slist *headers = NULL;
        
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        char auth_header[256];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", api_key);
        headers = curl_slist_append(headers, auth_header);

        curl_easy_setopt(curl, CURLOPT_URL, target);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_string);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            printf("[JARVIS ERREUR] L'envoi a échoué : %s\n", curl_easy_strerror(res));
        } else {
            printf("[JARVIS NETWORK] Succès ! Données envoyées à %s\n", target);
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    json_object_put(root); 
}