/*
 * ADS.c
 *
 *  Created on: Aug 31, 2023
 *      Author: divaldo
 */

#include "ADS.h"

int8_t ads_reset(uint8_t connector)
{
	uint8_t data_send[2];
	int8_t retVal;

	data_send[0]= 0x00;
	data_send[1]= 0x06;
	retVal = i2c_sensor_write(connector, ADS_ADDRESS, data_send, 2, 100);
	return retVal;
}

int8_t ads_init(uint8_t connector, ADS_Sampling_rate sampling_rate, ADS_gain gain, uint8_t continous_meas)
{
	uint8_t data_send = 0x80; //set st/!drdy to initiate measurement if not continous
	int8_t retVal;

	if (continous_meas != 1)
	{
		data_send |= (1 << 4);
	}

	switch(sampling_rate)
	{
		case SPS_15:
			data_send |= (1 << 3);
			data_send |= (1 << 2);
			break;
		case SPS_30:
			data_send |= (1 << 3);
			break;
		case SPS_60:
			data_send |= (1 << 2);
			break;
		case SPS_240:
			break;
	}

	switch(gain)
	{
		case GAIN1:
			break;
		case GAIN2:
			data_send |= (1 << 0);
			break;
		case GAIN4:
			data_send |= (1 << 1);
			break;
		case GAIN8:
			data_send |= (1 << 0);
			data_send |= (1 << 1);
			break;
	}
	//printf("ADS Config: %#02x\n", data_send);
	retVal = i2c_sensor_write(connector, ADS_ADDRESS, &data_send, 1, 100);
	return retVal;
}

int8_t ads_read_all(uint8_t connector, int16_t *data, uint8_t *current_config)
{
	uint8_t receive_data[3] = {1, 1, 1};
	int8_t retVal = i2c_sensor_read(connector, ADS_ADDRESS, receive_data, 3, 100);

	*data = ((receive_data[0] << 8) | (receive_data[1] << 0));
	*current_config = receive_data[2];

	return retVal;
}

int8_t ads_read_data(uint8_t connector, int16_t *data)
{
	uint8_t receive_data[2] = {1, 1};
	int8_t retVal = i2c_sensor_read(connector, ADS_ADDRESS, receive_data, 2, 100);

	*data = receive_data[0] << 8;
	*data |= receive_data[1];

	return retVal;
}

uint8_t ads_is_measurement_done(uint8_t *config)
{
	return !(*config >> 7) & 0x01;
}

//wait and block until a valid measurement
int8_t ads_measure_single(uint8_t connector, ADS_Sampling_rate sampling_rate, ADS_gain gain, int16_t *data)
{
	uint8_t config = 0x00;
	int8_t retVal = ads_init(connector, sampling_rate, gain, 0);
	if (retVal != HAL_OK)
	{
		return retVal;
	}

	do
	{
		retVal = ads_read_all(connector, data, &config);
		if (retVal != HAL_OK)
		{
			return retVal;
		}
	} while (!ads_is_measurement_done(&config));

	return retVal;
}

float ads_get_mV(int16_t *data, ADS_Sampling_rate sampling_rate, ADS_gain gain)
{
	float res_div;
	float gain_calc;
	float mv = 0.0f;
	switch(sampling_rate)
	{
		case SPS_15:
			res_div = 32768.0f;
			break;
		case SPS_30:
			res_div = 16384.0f;
			break;
		case SPS_60:
			res_div = 8192.0f;
			break;
		case SPS_240:
			res_div = 2048.0f;
			break;
	}

	switch(gain)
	{
		case GAIN1:
			gain_calc = 1.0f;
			break;
		case GAIN2:
			gain_calc = 2.0f;
			break;
		case GAIN4:
			gain_calc = 4.0f;
			break;
		case GAIN8:
			gain_calc = 8.0f;
			break;
	}

	mv = ((float)*data * (ADS1110_REF / res_div)) / gain_calc;
	return mv;
}
