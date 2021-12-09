/*
* DownlinkHandler.c
* Author: IoT Team (Bogdan, Gosia, Claudiu)
*/

#include "DownlinkHandler.h"
#include <lora_driver.h>
#include <message_buffer.h>
#include <rc_servo.h>



//uint16_t openWindow;
static lora_driver_payload_t downlinkPayload;

void lora_downlink_handler_task(void* messageBuffer);

void DownLinkHandler_Create(uint16_t lora_handler_task_priority, MessageBufferHandle_t downLinkMessageBuffer){
	
	printf("downlink 1");
	xTaskCreate(
	lora_downlink_handler_task
	, "LoRaWAN Handler Down link"
	, configMINIMAL_STACK_SIZE + 200
	, (void*)downLinkMessageBuffer
	, lora_handler_task_priority
	, NULL);
}



void lora_downlink_handler_task(void* downLinkMessageBuffer){
	for(;;){
		lora_DownLinkHandler_StartTask((MessageBufferHandle_t) downLinkMessageBuffer);
	}
}


void lora_DownLinkHandler_StartTask(MessageBufferHandle_t downLinkMessageBuffer){
	
	printf("downlink 2");
	
	rc_servo_setPosition(0,-100);
	rc_servo_setPosition(0, 100);
	rc_servo_setPosition(0,-100);
	
	printf("servo work");
	xMessageBufferReceive(downLinkMessageBuffer, &downlinkPayload, sizeof(lora_driver_payload_t), portMAX_DELAY);
	printf("DOWN LINK: from port: %d with %d bytes received!", downlinkPayload.portNo, downlinkPayload.len);
	if (1 == downlinkPayload.len){
		//openWindow = (downlinkPayload.bytes[0]);
		//printf("down link %d", openWindow);
	}
	//} else {
	//printf("downlink Timeout\n");
	//	}
}
