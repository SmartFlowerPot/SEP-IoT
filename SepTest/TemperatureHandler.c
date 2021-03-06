/*
* TemperatureHandler.c
* This class is to be used for handling reading sensor data from the Temperature and Humidity sensors.
* It creates a task that reads periodically all of the data, and makes use of the SharedSensorData.c file to protected the values.
*
* Author: IoT Team (Bogdan, Gosia, Claudiu)
*/

#include <stdint.h>
#include <hih8120.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "TemperatureHandler.h"
#include "SharedSensorData.h"
#include "SharedPrintf.h"

EventGroupHandle_t group_start;
EventBits_t ready_bit;

void startReading(void* self);

typedef struct TemperatureHandler{
	float temperature;
	uint16_t humidity;
} TemperatureHandler;

/*
* Create the task
*/
void temperature_handler_init(Temperature_t self, uint16_t priority){
	xTaskCreate(startReading,
	 "Temperature and humidity task",
	  configMINIMAL_STACK_SIZE,
	  (void *) self, 
	  priority, 
	  NULL);
}

/*
* create the struct, allocate memory, and initialize the sensor
*/
Temperature_t createTemp(uint16_t priority, EventGroupHandle_t taskBits, EventBits_t bit){
	Temperature_t new_measure = malloc(sizeof(TemperatureHandler));
	if(new_measure == NULL){
		return NULL;
	}
	new_measure ->temperature = 0.0;
	new_measure ->humidity = 0;
	
	group_start = taskBits;
	ready_bit = bit;
	
	if (HIH8120_OK == hih8120_initialise())
	{
		print_sharedf("Temperature and humidity sensor initialized.");
	}
	
	temperature_handler_init(new_measure, priority);
	return new_measure;
}

/*
* set data using the shared sensor data c file
*/
void measureTempAndHum(Temperature_t self){
	self->temperature = hih8120_getTemperature();
	self->humidity = hih8120_getHumidityPercent_x10()/10;
	
	set_temp_hum(self->temperature, self->humidity);
	xEventGroupSetBits(group_start, ready_bit);
}

/*
* start reading data
*/
void startReading(void* self){
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(15000UL);
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		//wait 15 seconds
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		//wake-up sensor
		if (HIH8120_OK != hih8120_wakeup())
		{
			print_sharedf("Temp task failed to work!");
		}
		
		//give the sensor time to wake up
		vTaskDelay(pdMS_TO_TICKS(70));
		
		hih8120_measure(); //measure temperature and humidity
		vTaskDelay(pdMS_TO_TICKS(15)); //wait for the measuring to be finished
			
		measureTempAndHum((Temperature_t) self);
	}
}

