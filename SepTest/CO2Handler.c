/*
* CO2Handler.c
* This class is to be used for handling reading sensor data from the CO2 sensor.
* It creates a task that reads periodically all of the data, and makes use of the SharedSensorData.c file to protected the values.
*
* Author: IoT Team (Bogdan, Gosia, Claudiu)
*/
#include <stdint.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <mh_z19.h>

#include "CO2Handler.h"
#include "SharedPrintf.h"
#include "SharedSensorData.h"
#include "SharedPrintf.h"

EventGroupHandle_t group_start;
EventBits_t ready_bit;

mh_z19_returnCode_t rc;
uint16_t ppm;

/*
* Declaring the task.
*/
void startReadingCO2(void* self);

/*
* Define struct.
*/
typedef struct CO2Handler{
	uint16_t CO2;
} CO2Handler;


/*
* Callback function called by the driver when the new CO2 measure is received.
*/
void CO2_callback(uint16_t callback){
	ppm = callback;
}

/*
* Method used for setting bits within an RTOS event group and for creating task.
*/
void CO2_handler_init(uint16_t CO2_task_priority, CO2_t self){

	xTaskCreate(
	startReadingCO2
	, "CO2Task"
	, configMINIMAL_STACK_SIZE + 100
	, (void*)self
	, CO2_task_priority
	, NULL);
	
}

/*
* Method used for allocating memory for CO2 struct and returning it, initializing the CO2 value to 0, setting groups and bits,
* initializing the driver, injecting callback and creating a CO2 task.
*/
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
	
	mh_z19_initialise(ser_USART3);
	print_sharedf("CO2 sensor initialized");
	mh_z19_injectCallBack(CO2_callback);
	
	CO2_handler_init(priority, new_measure);
	return new_measure;
}


/*
* Task for taking the CO2 measurements and setting the CO2 value accordingly.
*/
void startReadingCO2(void* self) {
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(16000UL);
	xLastWakeTime = xTaskGetTickCount();
	for (;;)
	{
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
	
			rc = mh_z19_takeMeassuring();
			if (rc != MHZ19_OK){
				// Something went wrong
				} else {
				((CO2_t)self)->CO2 = ppm;
				set_co2(ppm);
				xEventGroupSetBits(group_start, ready_bit);
			}
	}
}