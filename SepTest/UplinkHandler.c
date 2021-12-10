/*
* CFile1.c
*
* Created: 11-Nov-21 9:28:17 AM
*  Author: cirst and Gosia
*/

#include <lora_driver.h>
#include "TemperatureHandler.h"
#include "CO2Handler.h"
#include "LightHandler.h"
#include "SharedSensorData.h"
#include "SharedPrintf.h"
#include <math.h>
#include <stdio.h>
#include <rc_servo.h>

#define LORA_appEUI "9276B3CF3B069355"
#define LORA_appKEY "84860CBA5C5116F9EC56E1B4346CA899"

Temperature_t temperatureAndHumidity;
CO2_t co2;
LightHandler_t light_handler;
static lora_driver_payload_t _uplink_payload;
MessageBufferHandle_t downlinkBuffer;

void lora_handler_task(void* pvParameters);

void lora_handler_initialize(uint16_t lora_handler_task_priority, MessageBufferHandle_t downLinkMessageBufferHandle){
	
	//temperatureAndHumidity = temperatureObject;
	//co2 = co2Object;
	//light_handler = lightObject;
	downlinkBuffer = downLinkMessageBufferHandle;
	xTaskCreate(
	lora_handler_task
	, "LoRaWAN Hand"
	, configMINIMAL_STACK_SIZE + 200
	, NULL
	, lora_handler_task_priority
	, NULL);
}


static void _lora_setup(void)
{
	char _out_buf[20];
	lora_driver_returnCode_t rc;
	//status_leds_slowBlink(led_ST2); // OPTIONAL: Led the green led blink slowly while we are setting up LoRa

	// Factory reset the transceiver
	printf("FactoryReset >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_rn2483FactoryReset()));
	
	// Configure to EU868 LoRaWAN standards
	printf("Configure to EU868 >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_configureToEu868()));

	// Get the transceivers HW EUI
	rc = lora_driver_getRn2483Hweui(_out_buf);
	printf("Get HWEUI >%s<: %s\n",lora_driver_mapReturnCodeToText(rc), _out_buf);

	// Set the HWEUI as DevEUI in the LoRaWAN software stack in the transceiver
	printf("Set DevEUI: %s >%s<\n", _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setDeviceIdentifier(_out_buf)));

	// Set Over The Air Activation parameters to be ready to join the LoRaWAN
	printf("Set OTAA Identity appEUI:%s appKEY:%s devEUI:%s >%s<\n", LORA_appEUI, LORA_appKEY, _out_buf, lora_driver_mapReturnCodeToText(lora_driver_setOtaaIdentity(LORA_appEUI,LORA_appKEY,_out_buf)));

	// Save all the MAC settings in the transceiver
	printf("Save mac >%s<\n",lora_driver_mapReturnCodeToText(lora_driver_saveMac()));

	// Enable Adaptive Data Rate
	printf("Set Adaptive Data Rate: ON >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_setAdaptiveDataRate(LORA_ON)));

	// Set receiver window1 delay to 500 ms - this is needed if down-link messages will be used
	printf("Set Receiver Delay: %d ms >%s<\n", 500, lora_driver_mapReturnCodeToText(lora_driver_setReceiveDelay(500)));

	// Join the LoRaWAN
	uint8_t maxJoinTriesLeft = 10;
	
	do {
		rc = lora_driver_join(LORA_OTAA);
		printf("Join Network TriesLeft:%d >%s<\n", maxJoinTriesLeft, lora_driver_mapReturnCodeToText(rc));

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
		// Make the green led steady
		//status_leds_ledOn(led_ST2); // OPTIONAL
	}
	else
	{
		// Something went wrong
		// Turn off the green led
		//status_leds_ledOff(led_ST2); // OPTIONAL
		// Make the red led blink fast to tell something went wrong
		//status_leds_fastBlink(led_ST1); // OPTIONAL

		// Lets stay here
		while (1)
		{
			taskYIELD();
		}
	}
}

void lora_handler_task(void* pvParameters){
	
	// Hardware reset of LoRaWAN transceiver
	lora_driver_resetRn2483(1);
	vTaskDelay(2);
	lora_driver_resetRn2483(0);
	// Give it a chance to wakeup
	vTaskDelay(150);

	lora_driver_flushBuffers(); // get rid of first version string from module after reset!

	_lora_setup();
	
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = pdMS_TO_TICKS(30000UL); // Upload message every 5 minutes (300000 ms)
	xLastWakeTime = xTaskGetTickCount();
	
	
	for(;;){
		
		xTaskDelayUntil(&xLastWakeTime, xFrequency);
		_uplink_payload.len = 7;
		_uplink_payload.portNo = 1;
		
		
		double temp = (double) get_temp_val();
		uint16_t humidity = get_humidity_val();
		uint16_t co2_val = get_co2_mutex();
		uint16_t light_val = get_light_mutex();
				
	
		double val1=0;
		double val2=0;
		val2 = modf(temp, &val1);
		val2 = val2 * 100;
		printf("\ntemp: %f", temp);
		_uplink_payload.bytes[0] = (int) val1;
		_uplink_payload.bytes[1] = (int) val2;
		
		//humidity
		printf("\nhumidity: %d", humidity);
		_uplink_payload.bytes[2] = humidity;
		
		//co2
		printf("\nco2: %d", co2_val);
		_uplink_payload.bytes[3] = co2_val >> 8;
		_uplink_payload.bytes[4] = co2_val & 0xFF;
		
		//light
		printf("\nlight in lux: %d", light_val);
		_uplink_payload.bytes[5] = light_val >> 8;
		_uplink_payload.bytes[6] = light_val & 0xFF;
		
	
		
		printf("Upload Message >%s<\n", lora_driver_mapReturnCodeToText(lora_driver_sendUploadMessage(false, &_uplink_payload)));
		
		lora_driver_payload_t downlinkPayload;
		xMessageBufferReceive(downlinkBuffer, &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY);
		printf("DOWN LINK: from port: %d with %d bytes received!", downlinkPayload.portNo, downlinkPayload.len);
		if (1 == downlinkPayload.len){
		}
	}
	
}