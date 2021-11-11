/*
 * TemperatureHandler.c
 *
 * Created: 11-Nov-21 10:09:36 AM
 *  Author: cirst
 */ 
//Humidity  and temperature sensor
#include <stdint.h>
#include <hih8120.h>

#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>


#include "TemperatureHandler.h"


void startReading(void* self);

typedef struct TemperatureHandler{
	float temperature;
	} TemperatureHandler;
	

Temperature_t createTemp(){
	Temperature_t new_measure = malloc(sizeof(TemperatureHandler));
	if(new_measure == NULL){
		return NULL;
	}
	new_measure ->temperature = 0.0;
	
	if (HIH8120_OK == hih8120_initialise())
	{
		puts("Temp sensor initialized.\n");
	}
	
	temperature_handler_init(new_measure);
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
		vTaskDelay(pdMS_TO_TICKS(70));
		hih8120_measure(); //measure temperature and humidity
		vTaskDelay(pdMS_TO_TICKS(15)); //wait for the measuring to be finished
		
		measureTemp((Temperature_t) self);
	}
}

void measureTemp(Temperature_t self){
	self->temperature += hih8120_getTemperature();
	printf("%f", self->temperature);
}

void temperature_handler_init(Temperature_t self){
	
	xTaskCreate(startReading, "Temperature task", configMINIMAL_STACK_SIZE,(void *) self,3, NULL);
}

void start(){
	
}	