#include <stdio.h>
#include <avr/io.h>
#include <tsl2591.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>

// Needed for LoRaWAN
#include <lora_driver.h>
#include <status_leds.h>

#include "TemperatureHandler.h"
#include "CO2Handler.h"
#include "LightHandler.h"
#include "event_groups.h"

#include "SharedSensorData.h"
#include "UplinkHandler.h"
#include "DownlinkHandler.h"
#include <message_buffer.h>

//Shared print
#include "SharedPrintf.h";

#include <rc_servo.h>


// define semaphore handle
SemaphoreHandle_t xMutexSemaphore;
EventGroupHandle_t taskReadyBits = NULL;
MessageBufferHandle_t downLinkMessageBufferHandle = NULL;

Temperature_t temperature_sensor;
LightHandler_t lighthandler;
CO2_t co2_sensor;

#define BIT_TEMP_READY (1 << 0)
#define BIT_CO2_READY (1 << 1)
#define BIT_LIGHT_READY (1 << 2)



void create_tasks_and_semaphores(void)
{
	// Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
	// because it is sharing a resource, such as the Serial port.
	// Semaphores should only be used whilst the scheduler is running, but we can set it up here.
	if ( xMutexSemaphore == NULL )  // Check to confirm that the Semaphore has not already been created.
	{
		xMutexSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore.
		if ( ( xMutexSemaphore ) != NULL )
		{
			xSemaphoreGive( ( xMutexSemaphore ) );  // Make the mutex available for use, by initially "Giving" the Semaphore.
		}
	}
	
	create_shared_printf();
	createTasksForSensors();
	create_semaphore_mutex_and_sensors(temperature_sensor);
	DownLinkHandler_Create(4, downLinkMessageBufferHandle);
	lora_handler_initialize(2, temperature_sensor, co2_sensor, xMutexSemaphore, lighthandler);	
}

void createTasksForSensors(){
	temperature_sensor = createTemp(3, taskReadyBits, BIT_TEMP_READY, xMutexSemaphore);
	lighthandler = createLightSensor(3, taskReadyBits, BIT_LIGHT_READY, xMutexSemaphore);
	co2_sensor = createCO2(3, taskReadyBits, BIT_CO2_READY, xMutexSemaphore);
	
}

void initializeSystem(){
	// initialize ready bits
	taskReadyBits = xEventGroupCreate();
	
	// Make it possible to use stdio on COM port 3 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	
	
	rc_servo_initialise();
	// Method for tasks and semaphore
	downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t)*1);
	lora_driver_initialise(ser_USART1, downLinkMessageBufferHandle);
	create_tasks_and_semaphores();
}

int main(void){
	initializeSystem();
	vTaskStartScheduler();
	while(1){}
}