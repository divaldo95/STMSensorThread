/*
 * ADS.h
 *
 *  Created on: Aug 31, 2023
 *      Author: divaldo
 */

#ifndef ADS1110_ADS_H_
#define ADS1110_ADS_H_

#include "../i2c_mux/i2c_mux.h"
#include "sensor_config.h"

#define ADS_ADDRESS		0x4B //ED3 | 1001 011
#define ADS1110_REF     2048.0f

typedef enum
{
	SPS_15 = 0,
	SPS_30 = 1,
	SPS_60 = 2,
	SPS_240 = 3
} ADS_Sampling_rate;

typedef enum
{
	GAIN1 = 0,
	GAIN2 = 1,
	GAIN4 = 2,
	GAIN8 = 3
} ADS_gain;


int8_t 	ads_reset(uint8_t connector);
int8_t 	ads_init(uint8_t connector, ADS_Sampling_rate sampling_rate, ADS_gain gain, uint8_t continous_meas);
int8_t 	ads_read_all(uint8_t connector, int16_t *data, uint8_t *current_config);
int8_t 	ads_read_data(uint8_t connector, int16_t *data);
uint8_t ads_is_measurement_done(uint8_t *config);
int8_t 	ads_measure_single(uint8_t connector, ADS_Sampling_rate sampling_rate, ADS_gain gain, int16_t *data);
float 	ads_get_mV(int16_t *data, ADS_Sampling_rate sampling_rate, ADS_gain gain);

#endif /* ADS1110_ADS_H_ */
