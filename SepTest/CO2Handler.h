/*
 * CO2Handler.h
 *  Author: IoT Team (Bogdan, Gosia, Claudiu)
*/


#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <semphr.h>
#include "event_groups.h"

typedef struct CO2Handler* CO2_t;

CO2_t createCO2(uint16_t priority, EventGroupHandle_t taskBits, EventBits_t bit, SemaphoreHandle_t mutex);
uint16_t getCO2(CO2_t self);
  