/*
* LightHandler.c
* This class is to be used for handling reading sensor data from the Light sensor.
* It creates a task that reads periodically all of the data, and makes use of the SharedSensorData.c file to protected the values.
*
* Author: IoT Team (Bogdan, Gosia, Claudiu)
*/

#include <stdint.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <tsl2591.h>

#include "LightHandler.h"
#include "SharedSensorData.h"
#include "SharedPrintf.h"

EventGroupHandle_t group_start;
EventBits_t ready_bit;
uint16_t lux_val = 0;
/*
* Declaring the task and callback for lux value.
*/
void startReadingLight(void* self);
void tsl2591Callback(tsl2591_returnCode_t ReturnCode, LightHandler_t self);

/*
* Define struct.
*/
typedef struct LightHandler{
	uint16_t lux;
} LightHandler;

/*
* Method used for setting bits within an RTOS event group and for creating task.
*/
void createLightTask(UBaseType_t light_priority, LightHandler_t self){
	xEventGroupSetBits(group_start, ready_bit);
	
	xTaskCreate(startReadingLight
	, "Light task"
	, configMINIMAL_STACK_SIZE + 200,
	(void*) self,
	light_priority,
	NULL);
}

/*
* Callback function called when the new lux data is fetched.
*/
void tsl2591Callback(tsl2591_returnCode_t rc, LightHandler_t self)
{
	
	float _lux;
	switch (rc)
	{
		case TSL2591_DATA_READY:
		if ( TSL2591_OK == (rc = tsl2591_getLux(&_lux)) )
		{
			lux_val = _lux;
			//setting data using the shared sensor data c file
			set_light(_lux);
			xEventGroupSetBits(group_start, ready_bit);
			self ->lux = _lux;
			
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			print_sharedf("Lux overflow - change gain and integration time\n"); //switch
		}
		break;
		
		case TSL2591_OK:
		// Last command performed successful
		break;
		
		case TSL2591_DEV_ID_READY:
		// Dev ID now fetched
		break;
		
		default:
		break;
	}
}

/*
* Method used for allocating memory for LightHandler struct and returning it, initializing the lux value to 0, setting groups and bits,
* initializing the driver, injecting callback, enabling the driver and creating a light task.
*/
LightHandler_t createLightSensor(UBaseType_t light_priority, EventGroupHandle_t eventBits, EventBits_t bits)
{
	LightHandler_t new_measure = calloc(1, sizeof(LightHandler));
	
	if(new_measure == NULL){
		return NULL;
	}
	new_measure -> lux = 0;
	ready_bit = bits;
	group_start = eventBits;
	
	//initialize sensor with the callback function
	if ( TSL2591_OK == tsl2591_initialise(tsl2591Callback))
	{
		print_sharedf("Light sensor initialized");
	}
	
	//enable sensor
	if ( TSL2591_OK != tsl2591_enable() )
	{
		//some error handling here
	}
	createLightTask(light_priority, new_measure);
	return new_measure;
}

/*
* Function used for measuring
*/
void measure_light(LightHandler_t self){
		if ( TSL2591_OK != tsl2591_fetchData() )
		{
			// Something went wrong
			// Investigate the return code further
		}
		else
		{
			//The light data will be ready after the driver calls the call back function with
			tsl2591Callback(TSL2591_DATA_READY, self);
		}
}

/*
* Reading task for sensor
*/
void startReadingLight(void* self){
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(15000UL);
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		//wait 15 seconds to read
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		measure_light((LightHandler_t) self);
	}
}

