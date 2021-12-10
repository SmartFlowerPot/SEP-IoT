/*
 * DownlinkHandler.h
 *
 * Created: 12/8/2021 10:16:27 AM
 *  Author: Gosia, Claudiu, Bogdan
 */ 


#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <ATMEGA_FreeRTOS.h>
#include <lora_driver.h>
#include <status_leds.h>
#include <message_buffer.h>

void DownLinkHandler_Create(uint16_t lora_handler_task_priority, MessageBufferHandle_t downLinkMessageBufferHandle);
