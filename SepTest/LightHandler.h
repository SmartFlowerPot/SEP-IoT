
/*
 * LightHandler.h
 *
 * Created: 12/8/2021 1:13:34 PM
 *  Author: Claudiu, Bogdan, Gosia
 */ 
#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <semphr.h>
#include "event_groups.h"


typedef struct LightHandler* LightHandler_t;

LightHandler_t createLightSensor(uint16_t priority, EventGroupHandle_t eventBits, EventBits_t bits, SemaphoreHandle_t mutex);
uint16_t getLight(LightHandler_t self);