/*
 * Created: 11/11/2021 5:40:47 PM
 *  Author: Gosia
 */ 
#include <semphr.h>

void lora_handler_initialize(uint16_t lora_handler_task_priority, Temperature_t temperatureObject, CO2_t co2Object, SemaphoreHandle_t mutex, LightHandler_t lightObject);
