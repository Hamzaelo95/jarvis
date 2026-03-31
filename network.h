#ifndef NETWORK_H
#define NETWORK_H
#include "hardware.h"

void send_data_to_api(CpuInfo *cpu, SystemMetrics *metrics, const char *target, const char *api_key);

#endif