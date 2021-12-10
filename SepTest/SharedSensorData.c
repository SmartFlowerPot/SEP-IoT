
/*
* SharedSensorData.c
* This class is to be used for setting and getting any sensor data necessary. 
* Each method or call is protected by the mutex semaphore by default, without need for external intervention.
*
* Created: 12/9/2021 12:08:32 PM
* Author: Gosia
*/
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include "SharedSensorData.h"
#include "TemperatureHandler.h"
#include "SharedPrintf.h"
#include "CO2Handler.h"
#include "LightHandler.h"
#include <stdlib.h>

Temperature_t tempHum;
CO2_t co2;
LightHandler_t light;
SemaphoreHandle_t xMutexSemaphore;

/*
* Method used to initialize pointers to the necessary structs and initializing the mutex for the shared data
*/
void create_semaphore_mutex_and_sensors(Temperature_t temperatureObject, CO2_t co2Object, LightHandler_t lightObject){
	if (xMutexSemaphore == NULL){
		xMutexSemaphore = xSemaphoreCreateMutex();
	}
	
	tempHum = temperatureObject;
	co2 = co2Object;
	light = lightObject;
}

/*
* Method used to set the temperature and humidity, protected by the mutex 
*/
void set_temp_hum_mutex(){
	
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		temp_hum_set(tempHum);
		xSemaphoreGive(xMutexSemaphore);
		} else{
		print_sharedf("The mutex could not be obtained.");
	}
}

/*
* Getter for the humidity, protected by the mutex
*/
uint16_t get_humidity_val(){
	uint16_t tmp;

	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		tmp = getHumidity(tempHum);
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
float get_temp_val(){
	float tmp;
	
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		tmp = getTemperature(tempHum);
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
void set_co2_mutex(){
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		set_co2(co2);
		xSemaphoreGive(xMutexSemaphore);
		} else{
		print_sharedf("The mutex could not be obtained.");
	}
}

/*
* Getter for co2, protected by the mutex
*/
uint16_t get_co2_mutex(){
	uint16_t tmp;
	
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		tmp = getCO2(co2);
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
void set_light_mutex(){
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		setLight(light);
		xSemaphoreGive(xMutexSemaphore);
		} else{
		print_sharedf("The mutex could not be obtained.");
	}
}

/*
* Getter for the light levels, protected by the mutex
*/
uint16_t get_light_mutex(){
	uint16_t tmp;
	
	if(xSemaphoreTake(xMutexSemaphore, (TickType_t) 300) == pdTRUE){
		tmp = getLight(light);
		xSemaphoreGive(xMutexSemaphore);
		return tmp;
		} else{
		print_sharedf("The mutex could not be obtained.");
		return 0;
	}
}