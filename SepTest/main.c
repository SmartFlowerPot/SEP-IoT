/*
* main.c
* Author : IHA
*
* Example main file including LoRaWAN setup
* Just for inspiration :)
*/

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

//Humidity sensor
#include <hih8120.h>

//C02 sensor
#include <mh_z19.h>

//Display drivers
#include <display_7seg.h>


// define Tasks
void tempRead( void *pvParameters );

// define semaphore handle
SemaphoreHandle_t xTestSemaphore;

// Prototype for LoRaWAN handler
void lora_handler_initialise(UBaseType_t lora_handler_task_priority);

/*-----------------------------------------------------------*/
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
	xTaskCreate(tempRead, "Task 3 Temp", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
}

void tempRead( void *pvParameters ){
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 500/portTICK_PERIOD_MS; // 500 ms
	// Initialize the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
	float temperature = 0.0;
	int humidity = 0;
	for(;;)
	{
		//puts("it works");
		xTaskDelayUntil( &xLastWakeTime, xFrequency );
		//puts("it works");
		if(hih8120_wakeup() != HIH8120_OK){
			puts("Error while waking up the sensor")
			} //wake up the sensor
		vTaskDelay(pdMS_TO_TICKS(60)); // Wait for sensor to wake up
		hih8120_measure(); //measure temperature and humidity
		vTaskDelay(pdMS_TO_TICKS(10)); //wait for the measuring to be finished
		if (hih8120_isReady() ==) {
			temperature = hih8120_getTemperature();
			printf("%f", temperature);
			display_7seg_display(temperature, 1);
			humidity = (uint8_t)hih8120_getHumidity();
			printf("%d", humidity);
			vTaskDelay(pdMS_TO_TICKS(1000));
			//display_7seg_display(humidity, 1);
		//}

		PORTA ^= _BV(PA7);
	}
	
}

/*-----------------------------------------------------------*/
void initialiseSystem()
{
	// Set output ports for LEDs used in the example
	DDRA |= _BV(DDA0) | _BV(DDA7);

	// Make it possible to use stdio on COM port 0 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	// Let's create some tasks
	create_tasks_and_semaphores();

	// vvvvvvvvvvvvvvvvv BELOW IS LoRaWAN initialization vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
	// Status LEDs driver
	//status_leds_initialise(5); // Priority 5 for internal task
	// Initialize the LoRaWAN driver without down-link buffer
	//lora_driver_initialise(1, NULL);
	// Create LoRaWAN task and start it up with priority 3
	//lora_handler_initialise(3);
}

/*-----------------------------------------------------------*/
int main(void)
{
	initialiseSystem(); // Must be done as the very first thing!!
	printf("Program started.");
	if ( HIH8120_OK != hih8120_initialise() )
	{
		//puts("Humidity&Temperature sensor initialized.");
	}
	// initialize display without any callback function
	display_7seg_initialise(NULL);
	// Power up the display
	display_7seg_powerUp();
	vTaskStartScheduler(); // Initialize and run the freeRTOS scheduler. Execution should never return from here.
	while (1){}
}

