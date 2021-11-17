/*
 * CO2Handler.h
 *  Author: IoT Team (Bogdan, Goshia, Claudiu)



#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "event_groups.h"

typedef struct CO2Handler* CO2_t;

CO2_t createCO2(uint16_t priority, EventGroupHandle_t taskBits, EventBits_t bit);
float getCO2(CO2_t self)
 */ 