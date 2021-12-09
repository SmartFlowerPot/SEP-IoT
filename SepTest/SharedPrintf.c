
/*
 * SharedPrintf.c
 *
 * Created: 12/9/2021 1:15:25 PM
 *  Author: Gosia
 */ 
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <stdio.h>
#include <stdarg.h>
#include <task.h>
#include "SharedPrintf.h"

SemaphoreHandle_t xMutexPrint;

void create_shared_printf(){
	if (xMutexPrint == NULL ){
		xMutexPrint = xSemaphoreCreateMutex();
	}
}

void print_sharedf(char* format, ...){
	va_list lst;
	if (xSemaphoreTake(xMutexPrint, (TickType_t) 300) == pdTRUE) {
		va_start(lst, format);
		vprintf(format, lst);
		xSemaphoreGive(xMutexPrint);
	}
	else {}
}