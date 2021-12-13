/*
* LightHandler.h
* Author: IoT Team (Bogdan, Gosia, Claudiu)
*/

#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <semphr.h>
#include <ATMEGA_FreeRTOS.h>
#include <event_groups.h>

typedef struct LightHandler* LightHandler_t;

LightHandler_t createLightSensor(UBaseType_t light_priority, EventGroupHandle_t eventBits, EventBits_t bits);
void setLight(LightHandler_t self);
uint16_t getLight(LightHandler_t self);