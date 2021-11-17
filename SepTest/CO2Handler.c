/*
 * CO2Handler.c
 *  Author: IoT Team (Bogdan, Goshia, Claudiu)
 

#include <stdint.h>
#include <hih8120.h>

#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "CO2Handler.h"

EventGroupHandle_t group_start;
EventBits_t ready_bit;

void startReading(void* self);

typedef struct CO2Handler{
	int CO2;
	} CO2Handler;
	

CO2_t createCO2(uint16_t priority, EventGroupHandle_t taskBits, EventBits_t bit)
{
	CO2_t new_measure = malloc(sizeof(CO2Handler));
	if(new_measure == NULL)
	{
		return NULL;
	}
	new_measure ->CO2 = 0;
	
	group_start = taskBits;
	ready_bit = bit;
	
	if(mh_z19_OK == mh_z19_initialise(ser_USART3))
	{
		puts("CO2 sensor initialized");
	}
	
	CO2_initialize_task(co2_task_priority, _new_reader);
	
	return new_measure;
}

void measureCO2(CO2_t self){
	self->CO2 = mh_z19_takeMeassuring();
	printf("%f \n", self->CO2);
}

void start_CO2_task(void* self) {
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(16000UL);
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		CO2_handler_task((CO2Handler_t)self);
	}
}

void CO2_initialize_task(UBaseType_t CO2_task_priority, CO2Handler_t self)
{

	xEventGroupSetBits(task_startGroup, _readyBit);

	xTaskCreate(
	start_CO2_task
	, "CO2Task"
	, configMINIMAL_STACK_SIZE + 100
	, (void*)self
	, CO2_task_priority
	, NULL);
}

void CO2_handler_task(CO2Handler_t self) {
	EventBits_t readyBits = xEventGroupWaitBits(task_startGroup,
	_readyBit,
	pdFALSE,
	pdTRUE,
	portMAX_DELAY);
}
*/ 