
/*
* SharedPrintf.c
* This class is to be used to print anything to the console in a protected manner, making use of mutexes.
* If anything is to be printed to the console, it should be done through the method print_sharedf, to avoid any problems.
* It uses a mutex to synchronize print-outs, without need of external intervention.
*
* Created: 12/9/2021 1:15:25 PM
* Author: Gosia
*/
#include <ATMEGA_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>
#include <stdio.h>
#include <stdarg.h>
#include <task.h>
#include "SharedPrintf.h"

SemaphoreHandle_t xMutexPrint;

/*
* Initialize the semaphore mutex used to print text to the terminal
*/
void create_shared_printf(){
	if (xMutexPrint == NULL ){
		xMutexPrint = xSemaphoreCreateMutex();
	}
}

/*
* Method used to print to the terminal, synchronized with the mutex
*/
void print_sharedf(char* format, ...){
	va_list lst;
	if (xSemaphoreTake(xMutexPrint, (TickType_t) 300) == pdTRUE) {
		va_start(lst, format);
		//needs testing
		vprintf(format, lst);
		printf("\n");
		xSemaphoreGive(xMutexPrint);
	}
	else {}
}