#pragma once

#include "timer.h"

void InitializeNetwork();
void GetPointCloud(int* size, float** points);

extern int global_delay;
extern std::atomic_int active_clients;