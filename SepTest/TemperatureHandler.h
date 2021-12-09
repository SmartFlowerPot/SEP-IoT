/*
 * TemperatureHandler.h
 *
 * Created: 11-Nov-21 10:12:59 AM
 *  Author: cirst
 */ 

#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <semphr.h>
#include "event_groups.h"

typedef struct TemperatureHandler* Temperature_t; 

Temperature_t createTemp(uint16_t priority, EventGroupHandle_t taskBits, EventBits_t bit, SemaphoreHandle_t mutex);
float getTemperature(Temperature_t self);
uint16_t getHumidity(Temperature_t self);
void set_values(Temperature_t self);