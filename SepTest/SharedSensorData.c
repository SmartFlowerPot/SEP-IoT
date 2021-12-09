
/*
 * SharedSensorData.c
 *
 * Created: 12/9/2021 12:08:32 PM
 *  Author: Gosia
 */ 
#include <ATMEGA_FreeRTOS.h>
#include "semphr.h"
#include "SharedSensorData.h"
#include "TemperatureHandler.h"
#include <stdlib.h>

Temperature_t tempHum;
SemaphoreHandle_t xMutexSemaphore;





void create_semaphore_mutex_and_sensors(Temperature_t temperatureObject){
	if ( xMutexSemaphore == NULL ){
		xMutexSemaphore = xSemaphoreCreateMutex();  
	}
	
	tempHum = temperatureObject;
	
}

void set_temp_hum_val(){
	if(xMutexSemaphore != NULL){
		if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
			set_values(tempHum);
			xSemaphoreGive(xMutexSemaphore);
	} else{
	printf("The mutex could not be obtained."); //replace with sharedprint
		}
	}
}

Temperature_t get_temp_hum_val(){
	if(xMutexSemaphore != NULL){
		if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
			return tempHum;
			} else{
			printf("The mutex could not be obtained."); //replace with sharedprint
		}
	}
}

uint16_t get_humidity_val(){
	if(xMutexSemaphore != NULL){
		if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
			return getHumidity(tempHum);
			} else{
			printf("The mutex could not be obtained."); //replace with sharedprint
		}
	}
}