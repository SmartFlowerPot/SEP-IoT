
/*
 * SharedSensorData.h
 *
 * Created: 12/9/2021 12:03:38 PM
 *  Author: Gosia
 */ 

#include "TemperatureHandler.h"
#include <stdint.h>


void create_semaphore_mutex_and_sensors(Temperature_t temperatureObject);
void set_temp_hum_val();
Temperature_t get_temp_hum_val();
uint16_t get_humidity_val();
