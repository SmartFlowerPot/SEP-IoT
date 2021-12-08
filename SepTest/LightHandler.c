
/*
 * LightHandler.c
 *
 * Created: 12/8/2021 1:19:37 PM
 *  Author: Claudiu, Bogdan, Gosia
 */ 

#include <stdint.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <tsl2591.h>

#include "LightHandler.h"

EventGroupHandle_t group_start;
EventBits_t ready_bit;
SemaphoreHandle_t xMutexSemaphore;

void startReadingLight(void* self);
void tsl2591Callback(tsl2591_returnCode_t ReturnCode, LightHandler_t self);

typedef struct LightHandler{
	uint16_t lux;
	} LightHandler;

LightHandler_t createLightSensor(uint16_t priority, EventGroupHandle_t eventBits, EventBits_t bits, SemaphoreHandle_t mutex)
{
	LightHandler_t new_measure = calloc(1, sizeof(LightHandler));
	
	if(new_measure == NULL){
		return NULL;
	}
	new_measure -> lux = 0;	
	ready_bit = bits;
	group_start = eventBits;
	xMutexSemaphore = mutex;
	
	if ( TSL2591_OK == tsl2591_initialise(tsl2591Callback) )
	{
		puts("Light sensor initialized");
	}
	
	if ( TSL2591_OK == tsl2591_enable() )
	{
		// The power up command is now send to the sensor - it can be powered down with a call to tsl2591_disable()
	}
	//createLightTask(priority, new_measure);
	return new_measure;
}

void createLightTask(uint16_t light_priority, LightHandler_t self){
	xEventGroupSetBits(group_start, ready_bit);
	xTaskCreate(startReadingLight, "Light task", configMINIMAL_STACK_SIZE + 200,(void*) self, light_priority, NULL);
}

void tsl2591Callback(tsl2591_returnCode_t rc, LightHandler_t self)
{
	uint16_t _tmp;
	float _lux;
	switch (rc)
	{
		case TSL2591_DATA_READY:
		if ( TSL2591_OK == (rc = tsl2591_getFullSpectrumRaw(&_tmp)) )
		{
			printf("\nFull Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("\nFull spectrum overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl259_getVisibleRaw(&_tmp)) )
		{
			printf("Visible Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Visible overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl2591_getInfraredRaw(&_tmp)) )
		{
			printf("Infrared Raw:%04X\n", _tmp);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Infrared overflow - change gain and integration time\n");
		}
		
		if ( TSL2591_OK == (rc = tsl2591_getLux(&_lux)) )
		{
			self->lux = _lux;
			printf("Lux: %5.4f\n", _lux);
		}
		else if( TSL2591_OVERFLOW == rc )
		{
			printf("Lux overflow - change gain and integration time\n");
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

void measure_light(LightHandler_t self){
	
	EventBits_t readyBits = xEventGroupWaitBits(group_start,
	ready_bit,
	pdFALSE,
	pdTRUE,
	portMAX_DELAY);
	
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

void startReadingLight(void* self){
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(15000UL);
	xLastWakeTime = xTaskGetTickCount();
	
	for(;;){
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		measure_light((LightHandler_t) self);
	}
}


uint16_t getLight(LightHandler_t self){
	return self->lux;
}

