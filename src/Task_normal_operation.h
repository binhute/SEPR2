#ifndef TASK_NORMAL_OPERATION_H
#define TASK_NORMAL_OPERATION_H

#include <config.h>

extern Preferences prefs;
extern SystemConfig cfg;

typedef struct {
    int limit;
    int price;   
} ElectricityTier;
long tieredElectricCalculate(int kwh, ElectricityTier tiers[], int size, bool includeVAT);
void TaskNormalOpe(void *pvParameters);

#endif