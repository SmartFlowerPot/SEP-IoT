
/*
 * SharedSensorData.h
 *
 * Created: 12/9/2021 12:03:38 PM
 *  Author: Gosia
 */ 

#include "TemperatureHandler.h"
#include "CO2Handler.h"
#include "LightHandler.h"
#include <stdint.h>


void create_semaphore_mutex_and_sensors(Temperature_t temperatureObject, CO2_t co2Object, LightHandler_t lightObject);
void set_temp_hum_mutex();
uint16_t get_humidity_val();
float get_temp_val();
void set_co2_mutex();
uint16_t get_co2_mutex();
void set_light_mutex();
uint16_t get_light_mutex();
