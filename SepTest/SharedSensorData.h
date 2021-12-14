
/*
 * SharedSensorData.h
 *
 * Created: 12/9/2021 12:03:38 PM
 *  Author: Gosia
 */ 



#include <stdint.h>


void create_semaphore_mutex_and_sensors();
void set_temp_hum(float temp, uint16_t hum);
uint16_t get_humidity();
float get_temp();
void set_co2(uint16_t co2);
uint16_t get_co2();
void set_light(uint16_t light_lux);
uint16_t get_light();
