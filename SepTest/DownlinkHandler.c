/*
* DownlinkHandler.c
* Class used to receive messages from the LoRaWAN Network, currently used to actuate the Servo motor that would open the window
*
* Author: IoT Team (Bogdan, Gosia, Claudiu)
*/

#include <lora_driver.h>
#include <message_buffer.h>
#include <rc_servo.h>
#include "DownlinkHandler.h"
#include "SharedPrintf.h"

static lora_driver_payload_t downlinkPayload;

void lora_downlink_handler_task(void* messageBuffer);

/*
* Function that creates the listener task
*/
void DownLinkHandler_Create(uint16_t lora_handler_task_priority, MessageBufferHandle_t downLinkMessageBuffer){
	xTaskCreate(
	lora_downlink_handler_task
	, "LoRaWAN Handler Down link"
	, configMINIMAL_STACK_SIZE + 200
	, (void*)downLinkMessageBuffer
	, lora_handler_task_priority
	, NULL);
}

/*
* Function that contains task body and accepts incoming messages to open/close the window
*/
void lora_DownLinkHandler_StartTask(MessageBufferHandle_t downLinkMessageBuffer){
	
	xMessageBufferReceive(downLinkMessageBuffer, &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY);
	print_sharedf("DOWN LINK: from port: %d with %d bytes received!", downlinkPayload.portNo, downlinkPayload.len);
	if (1 == downlinkPayload.len){
		if(downlinkPayload.bytes[0] == 1){
			rc_servo_setPosition(1,-100); // fully open
			print_sharedf("Window opened");
		}
		else{
			rc_servo_setPosition(1, 100); // fully close
			print_sharedf("Window closed");
		}
	}
}

/*
* Task method used to run the actual task
*/
void lora_downlink_handler_task(void* downLinkMessageBuffer){
	for(;;){
		lora_DownLinkHandler_StartTask((MessageBufferHandle_t) downLinkMessageBuffer);
	}
}

