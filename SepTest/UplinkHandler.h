/*
 * Created: 11/11/2021 5:40:47 PM
 *  Author: Gosia
 */ 

#include <semphr.h>
#include <event_groups.h>

void lora_handler_initialize(uint16_t lora_handler_task_priority, EventGroupHandle_t taskReadyBits, EventBits_t tempbit, EventBits_t co2bit, EventBits_t lightbit);
