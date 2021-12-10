#include <stdio.h>
#include <avr/io.h>
#include <tsl2591.h>
#include <ATMEGA_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio_driver.h>
#include <serial.h>
#include <lora_driver.h>
#include <message_buffer.h>
#include <rc_servo.h>

#include "TemperatureHandler.h"
#include "CO2Handler.h"
#include "LightHandler.h"
#include "event_groups.h"
#include "SharedSensorData.h"
#include "UplinkHandler.h"
#include "DownlinkHandler.h"
#include "SharedPrintf.h"

EventGroupHandle_t taskReadyBits = NULL;
MessageBufferHandle_t downLinkMessageBufferHandle = NULL;

Temperature_t temperature_sensor;
LightHandler_t lighthandler;
CO2_t co2_sensor;

#define BIT_TEMP_READY (1 << 0)
#define BIT_CO2_READY (1 << 1)
#define BIT_LIGHT_READY (1 << 2)

void createTasksForSensors(){
	temperature_sensor = createTemp(3, taskReadyBits, BIT_TEMP_READY);
	co2_sensor = createCO2(3, taskReadyBits, BIT_CO2_READY);
	lighthandler = createLightSensor(3, taskReadyBits, BIT_LIGHT_READY);
}

void create_tasks_and_semaphores(void)
{
	create_shared_printf();
	print_sharedf("Board started.");
	createTasksForSensors();
	create_semaphore_mutex_and_sensors(temperature_sensor, co2_sensor, lighthandler);
	DownLinkHandler_Create(4, downLinkMessageBufferHandle);
	lora_handler_initialize(2);
}

void initializeSystem(){
	// initialize ready bits
	taskReadyBits = xEventGroupCreate();
	
	// Make it possible to use stdio on COM port 3 (USB) on Arduino board - Setting 57600,8,N,1
	stdio_initialise(ser_USART0);
	
	rc_servo_initialise();
	// Method for tasks and semaphore
	downLinkMessageBufferHandle = xMessageBufferCreate(sizeof(lora_driver_payload_t)*2);
	lora_driver_initialise(ser_USART1, downLinkMessageBufferHandle);
	create_tasks_and_semaphores();
}

int main(void){
	initializeSystem();
	vTaskStartScheduler();
	while(1){}
}