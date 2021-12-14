/*
* SharedSensorData.c
* This class is to be used for setting and getting any sensor data necessary. 
* Each function or call is protected by the mutex semaphore by default, without need for external intervention.
*
* Created: 12/9/2021 12:08:32 PM
* Author: Gosia
*/

#include <stdlib.h>
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>

#include "SharedSensorData.h"
#include "SharedPrintf.h"

SemaphoreHandle_t xMutexSemaphore;

static float temperature_value = 0.0;
static uint16_t humidity_value = 0;
static uint16_t co2_value = 0;
static uint16_t light_value = 0;
/*
* Function used to initialize pointers to the necessary structs and initializing the mutex for the shared data
*/
void create_semaphore_mutex_and_sensors(){
	if (xMutexSemaphore == NULL){
		xMutexSemaphore = xSemaphoreCreateMutex();
	}
}

/*
* Function used to set the temperature and humidity, protected by the mutex 
*/
void set_temp_hum(float temp, uint16_t hum){
	
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		temperature_value = temp;
		humidity_value = hum;
		xSemaphoreGive(xMutexSemaphore);
		} else{
		print_sharedf("The mutex could not be obtained.");
	}
}

/*
* Getter for the humidity, protected by the mutex
*/
uint16_t get_humidity(){
	uint16_t tmp;

	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		tmp = humidity_value;
		xSemaphoreGive(xMutexSemaphore);
		return tmp;
		} else{
		print_sharedf("The mutex could not be obtained.");
		return 0;
	}
}

/*
* Getter for the temperature, protected by the mutex
*/
float get_temp(){
	float tmp;
	
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		tmp = temperature_value;
		xSemaphoreGive(xMutexSemaphore);
		return tmp;
		} else{
		print_sharedf("The mutex could not be obtained.");
		return 0.0;
	}
}

/*
* Setter for the co2, protected by the mutex
*/
void set_co2(uint16_t co2){
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		co2_value = co2;
		xSemaphoreGive(xMutexSemaphore);
		} else{
		print_sharedf("The mutex could not be obtained.");
	}
}

/*
* Getter for co2, protected by the mutex
*/
uint16_t get_co2(){
	uint16_t tmp;
	
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		tmp = co2_value;
		xSemaphoreGive(xMutexSemaphore);
		return tmp;
		} else{
		print_sharedf("The mutex could not be obtained.");
		return 0;
	}
}

/*
* Setter for light levels, protected by the mutex
*/
void set_light(uint16_t light_lux){
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		light_value = light_lux;
		xSemaphoreGive(xMutexSemaphore);
		} else{
		print_sharedf("The mutex could not be obtained.");
	}
}

/*
* Getter for the light levels, protected by the mutex
*/
uint16_t get_light(){
	uint16_t tmp;
	
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		tmp = light_value;
		xSemaphoreGive(xMutexSemaphore);
		return tmp;
		} else{
		print_sharedf("The mutex could not be obtained.");
		return 0;
	}
}