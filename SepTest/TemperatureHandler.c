
//Humidity  and temperature sensor
#include <stdint.h>
#include <hih8120.h>

#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "TemperatureHandler.h"

EventGroupHandle_t group_start;
EventBits_t ready_bit;

void startReading(void* self);

typedef struct TemperatureHandler{
	float temperature;
	} TemperatureHandler;
	

Temperature_t createTemp(uint16_t priority, EventGroupHandle_t taskBits, EventBits_t bit){
	Temperature_t new_measure = malloc(sizeof(TemperatureHandler));
	if(new_measure == NULL){
		return NULL;
	}
	new_measure ->temperature = 0.0;
	
	group_start = taskBits;
	ready_bit = bit;
	
	if (HIH8120_OK == hih8120_initialise())
	{
		puts("Temp sensor initialized.");
	}
	
	temperature_handler_init(new_measure, priority);
	return new_measure;
}

void startReading(void* self){
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(15000UL);
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		if (HIH8120_OK != hih8120_wakeup())
		{
			puts("Temp task failed to work!");
		}
		
		EventBits_t readyBits = xEventGroupWaitBits(group_start,
													ready_bit,
													pdFALSE,
													pdTRUE,
													portMAX_DELAY);
		
		vTaskDelay(pdMS_TO_TICKS(70));
		hih8120_measure(); //measure temperature and humidity
		vTaskDelay(pdMS_TO_TICKS(15)); //wait for the measuring to be finished
		
		measureTemp((Temperature_t) self);
	}
}

void measureTemp(Temperature_t self){
	self->temperature = hih8120_getTemperature();
	printf("%f \n", self->temperature);
}

void temperature_handler_init(Temperature_t self, uint16_t priority){
	xEventGroupSetBits(group_start, ready_bit);
	xTaskCreate(startReading, "Temperature task", configMINIMAL_STACK_SIZE,(void *) self, priority, NULL);
}

float getTemperature(Temperature_t self){
	return self->temperature;
}