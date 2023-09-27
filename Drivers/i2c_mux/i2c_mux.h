/*
 * i2c_mux.h
 *
 *  Created on: 2022. okt. 14.
 *      Author: zbiro
 */

#ifndef I2C_MUX_I2C_MUX_H_
#define I2C_MUX_I2C_MUX_H_

#include "main.h"
#include "i2c.h"
#include "gpio.h"
#include "../Common/DataStructures.h"


#define i2c_mux_address 0x70

static uint8_t i2c_skip_addresses[] = {0x3C, 0x70, 0x4A}; //oled, i2c_mux, ina?

int8_t i2c_mux_select(uint8_t connector);
int8_t i2c_sensor_read(uint8_t connector, uint8_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout);
int8_t i2c_sensor_write(uint8_t connector, uint8_t DevAddress, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
SensorType i2c_sensor_scan(uint8_t connector);
void enable_5v();
void disable_5v();


#endif /* I2C_MUX_I2C_MUX_H_ */
