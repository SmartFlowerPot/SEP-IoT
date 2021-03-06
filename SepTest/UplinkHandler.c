/*
* UplinkHandler.c
* This class is responsible for handling any networking required to send data to the LoRaWAN Network.
*
* Created: 11-Nov-21 9:28:17 AM
*  Author: Bogdan and Gosia
*/

#include <lora_driver.h>
#include "TemperatureHandler.h"
#include "CO2Handler.h"
#include "LightHandler.h"
#include "SharedSensorData.h"
#include "SharedPrintf.h"


#include <math.h>
#include <stdio.h>
#include <message_buffer.h>
#include <rc_servo.h>
#include <event_groups.h>

#define LORA_appEUI "9276B3CF3B069355"
#define LORA_appKEY "84860CBA5C5116F9EC56E1B4346CA899"

static lora_driver_payload_t _uplink_payload;

EventGroupHandle_t taskBits_ready;
EventBits_t tempbit_ready;
EventBits_t co2bit_ready;
EventBits_t lightbit_ready;

void lora_handler_task(void* pvParameters);

/*
* Function call used to start the networking task
*/
void lora_handler_initialize(uint16_t lora_handler_task_priority, EventGroupHandle_t taskReadyBits, 
EventBits_t tempbit, EventBits_t co2bit, EventBits_t lightbit){
	
	taskBits_ready = taskReadyBits;
	tempbit_ready = tempbit;
	co2bit_ready = co2bit;
	lightbit_ready = lightbit;
	
	xTaskCreate(
	lora_handler_task
	, "LoRaWAN Uplink"
	, configMINIMAL_STACK_SIZE + 200
	, NULL
	, lora_handler_task_priority
	, NULL);
}

/*
* Setup function that initializes the LoRaWAN Parameters
*/
static void _lora_setup(void)
{
	char _out_buf[20];
	lora_driver_returnCode_t rc;

	// Factory reset the transceiver
	lora_driver_rn2483FactoryReset();
	
	// Configure to EU868 LoRaWAN standards
	lora_driver_configureToEu868();

	// Get the transceivers HW EUI
	rc = lora_driver_getRn2483Hweui(_out_buf);

	// Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	lora_driver_setDeviceIdentifier(_out_buf);

	// Set Over The Air Activation parameters to be ready to join the LoRaWAN
	lora_driver_setOtaaIdentity(LORA_appEUI,LORA_appKEY,_out_buf);

	// Save all the MAC settings in the transceiver
	lora_driver_saveMac();

	// Enable Adaptive Data Rate
	lora_driver_setAdaptiveDataRate(LORA_ON);

	// Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	lora_driver_setReceiveDelay(500);

	// Join the LoRaWAN
	uint8_t maxJoinTriesLeft = 10;
	
	do {
		rc = lora_driver_join(LORA_OTAA);
		print_sharedf("Join Network Tries Left:%d >%s<", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

		if ( rc != LORA_ACCEPTED)
		{
			// Make the red led pulse to tell something went wrong
			// status_leds_longPuls(led_ST1); // OPTIONAL
			// Wait 5 sec and lets try again
			vTaskDelay(pdMS_TO_TICKS(5000UL));
		}
		else
		{
			break;
		}
	} while (--maxJoinTriesLeft);

	if (rc == LORA_ACCEPTED)
	{
		// Connected to LoRaWAN :-)
		print_sharedf("Connected to the LoRaWAN Network successfully.");
	}
	else
	{
		// Something went wrong
		// Lets stay here
		while (1)
		{
			taskYIELD();
		}
	}
}

/*
* Function task that stars sending data in the specified 5 minute interval
*/
void lora_handler_task(void* pvParameters){
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(300000UL); // Upload message every 5 minutes (300000 ms)
	xLastWakeTime = xTaskGetTickCount();
	// Hardware reset of LoRaWAN transceiver
	lora_driver_resetRn2483(1);
	vTaskDelay(2);
	lora_driver_resetRn2483(0);
	// Give it a chance to wakeup
	vTaskDelay(150);

	lora_driver_flushBuffers(); // get rid of first version string from module after reset!

	_lora_setup();
	
	for(;;){
		
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		_uplink_payload.len = 7;
		_uplink_payload.portNo = 1;
		
		EventBits_t readyBits = xEventGroupWaitBits(taskBits_ready,
		tempbit_ready&co2bit_ready&lightbit_ready,
		pdTRUE,
		pdTRUE,
		portMAX_DELAY);
		
		if((readyBits & (tempbit_ready&co2bit_ready&lightbit_ready)) == (tempbit_ready&co2bit_ready&lightbit_ready)){
			
			double temp = (double) get_temp();
			uint16_t humidity = get_humidity();
			uint16_t co2_val = get_co2();
			uint16_t light_val = get_light();
			
			if(temp == 0.0 || humidity == 0 || co2_val == 0){
				print_sharedf("Sensors are still calibrating %f, %d, %d", temp, humidity, co2_val);
			}
			else{
				double val1=0;
				double val2=0;
				val2 = modf(temp, &val1);
				val2 = val2 * 100;
				
				//temperature
				print_sharedf("Temperature: %f", temp);
				_uplink_payload.bytes[0] = (int) val1;
				_uplink_payload.bytes[1] = (int) val2;
				
				//humidity
				print_sharedf("Humidity: %d", humidity);
				_uplink_payload.bytes[2] = humidity;
				
				//co2
				print_sharedf("CO2: %d", co2_val);
				_uplink_payload.bytes[3] = co2_val >> 8;
				_uplink_payload.bytes[4] = co2_val & 0xFF;
				
				//light
				print_sharedf("Light in lux: %d", light_val);
				_uplink_payload.bytes[5] = light_val >> 8;
				_uplink_payload.bytes[6] = light_val & 0xFF;
				
				char* message = lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_uplink_payload));
				print_sharedf("Upload Message >%s<", message);
			}
		}
	}
	
}