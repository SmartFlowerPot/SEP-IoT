
 //* CO2Handler.c
 //*  Author: IoT Team (Bogdan, Goshia, Claudiu)
 

#include <stdint.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <mh_z19.h>

#include "CO2Handler.h"

EventGroupHandle_t group_start;
EventBits_t ready_bit;
SemaphoreHandle_t xMutexSemaphore;

mh_z19_returnCode_t rc;
uint16_t ppm;

void startReadingCO2(void* self);

typedef struct CO2Handler{
	uint16_t CO2;
	} CO2Handler;
	
	
void CO2_callback(uint16_t callback){
	ppm = callback;
}

CO2_t createCO2(uint16_t priority, EventGroupHandle_t taskBits, EventBits_t bit, SemaphoreHandle_t mutex)
{
	CO2_t new_measure = malloc(sizeof(CO2Handler));
	if(new_measure == NULL)
	{
		return NULL;
	}
	new_measure ->CO2 = 0;
	
	group_start = taskBits;
	ready_bit = bit;
	xMutexSemaphore = mutex;
	//maybe wait time here?
	mh_z19_initialise(ser_USART3);
	
	mh_z19_injectCallBack(CO2_callback);
	
	CO2_handler_init(priority, new_measure);
	return new_measure;
}

uint16_t getCO2(CO2_t self){
	return self->CO2;
}

void startReadingCO2(void* self) {
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(16000UL);
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		//need to set bits
		EventBits_t readyBits = xEventGroupWaitBits(group_start,
		ready_bit,
		pdFALSE,
		pdTRUE,
		portMAX_DELAY);
		
		rc = mh_z19_takeMeassuring();
		if (rc != MHZ19_OK){
			// Something went wrong
		} else {
			
			if(xMutexSemaphore != NULL){
				if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 10) == pdTRUE){
					((CO2_t)self)->CO2 = ppm;
					printf("CO2 %d", ppm);
					xSemaphoreGive(xMutexSemaphore);
				}
				else{
					printf("The mutex could not be obtained.");
				}
			}
			
		}
		
	}
}

void CO2_handler_init(uint16_t CO2_task_priority, CO2_t self){
	xEventGroupSetBits(group_start, ready_bit);

	xTaskCreate(
	startReadingCO2
	, "CO2Task"
	, configMINIMAL_STACK_SIZE + 100
	, (void*)self
	, CO2_task_priority
	, NULL);
	
	puts("CO2 initialized");
}


