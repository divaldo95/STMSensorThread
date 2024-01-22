#ifndef HDC2080_H
#define HDC2080_H

#include "../i2c_mux/i2c_mux.h"
#include "sensor_config.h"

#define         HDC2080_ADDRESS                         0x40
#define         HDC2080_MEAS_MODE_ADDRESS               0x0E
#define         HDC2080_CONFIGURATION_ADDRESS           0x0F
#define         HDC2080_TEMP_LOW_ADDRESS                0x00
#define         HDC2080_TEMP_HIGH_ADDRESS               0x01
#define         HDC2080_HUM_LOW_ADDRESS                 0x02
#define         HDC2080_HUM_HIGH_ADDRESS                0x03


typedef enum
{
  Temperature_Resolution_14_bit = 0,
  Temperature_Resolution_11_bit = 1,
  Temperature_Resolution_9_bit
} Temperature_Resolution;

typedef enum
{
  Humidity_Resolution_14_bit = 0,
  Humidity_Resolution_11_bit = 1,
  Humidity_Resolution_9_bit = 2
} Humidity_Resolution;

int8_t hdc2080_init(uint8_t connector, Temperature_Resolution temp_res, Humidity_Resolution hum_res);
int8_t hdc2080_measure(uint8_t connector, float* temperature, uint8_t* humidity);
#endif
