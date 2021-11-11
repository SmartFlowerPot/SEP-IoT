#include <stdio.h>
#include <avr/io.h>

#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include <stdio_driver.h>
#include <serial.h>

// Needed for LoRaWAN
#include <lora_driver.h>
#include <status_leds.h>

#include "TemperatureHandler.h"

// define semaphore handle
SemaphoreHandle_t xTestSemaphore;

Temperature_t temperature_sensor;



void create_tasks_and_semaphores(void)
{
	// Semaphores are useful to stop a Task proceeding, where it should be paused to wait,
	// because it is sharing a resource, such as the Serial port.
	// Semaphores should only be used whilst the scheduler is running, but we can set it up here.
	if ( xTestSemaphore == NULL )  // Check to confirm that the Semaphore has not already been created.
	{
		xTestSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore.
		if ( ( xTestSemaphore ) != NULL )
		{
			xSemaphoreGive( ( xTestSemaphore ) );  // Make the mutex available for use, by initially "Giving" the Semaphore.
		}
	}
	
	//temperature task
	//xTaskCreate(tempRead, "Task 3 Temp", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	createTasksForSensors();
}

void createTasksForSensors(){
	temperature_sensor = createTemp();
}

void initializeSystem(){
	// Make it possible to use stdio on COM port 3 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	// Method for tasks and semaphore
	create_tasks_and_semaphores();
}

int main(void){
	initializeSystem();
	vTaskStartScheduler();
	while(1){}
}