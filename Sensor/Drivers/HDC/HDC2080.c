/*
 * HDC1080.c
 *
 *  Created on: Aug 30, 2023
 *      Author: divaldo
 */


#include "HDC2080.h"

int8_t hdc2080_init(uint8_t connector, Temperature_Resolution temp_res, Humidity_Resolution hum_res)
{
	/* Temperature and Humidity are acquired in sequence, Temperature first
	 * Default:   Temperature resolution = 14 bit,
	 *            Humidity resolution = 14 bit
	 */

	/* Set the acquisition mode to measure both temperature and humidity by setting Bit[12] to 1 */
	uint8_t config_reg_value=0x00;
	uint8_t data_send[2];
	int8_t retVal;

	//set sampling to 5Hz, no need to init measurement, just read the registers
	config_reg_value = 0x70; //5Hz sampling
	data_send[0]= (HDC2080_MEAS_MODE_ADDRESS);
	data_send[1]= (config_reg_value);
	retVal = i2c_sensor_write(connector, HDC2080_ADDRESS, data_send, 2, 100);

	if ( retVal != HAL_OK )
	{
		return retVal;
	}

	config_reg_value=0x01;

	//00 - 14b | 01 - 11b | 10 - 9b | 11 - NA
	switch(temp_res)
	{
		case Temperature_Resolution_14_bit:
			//config_reg_value|= (1<<8);
			break;
		case Temperature_Resolution_11_bit:
			config_reg_value|= (1<<6);
			break;
		case Temperature_Resolution_9_bit:
			config_reg_value|= (1<<7);
			break;
	}

	//00 - 14b | 01 - 11b | 10 - 9b | 11 - NA
	switch(hum_res)
	{
		case Humidity_Resolution_14_bit:
			//config_reg_value|= (1<<8);
			break;
		case Humidity_Resolution_11_bit:
			config_reg_value|= (1<<4);
			break;
		case Humidity_Resolution_9_bit:
			config_reg_value|= (1<<5);
			break;
	}

	data_send[0]= (HDC2080_CONFIGURATION_ADDRESS);
	data_send[1]= (config_reg_value);

	//HAL_I2C_Mem_Write(hi2c_x,HDC_1080_ADD<<1,Configuration_register_add,I2C_MEMADD_SIZE_8BIT,data_send,3,1000);

	retVal = i2c_sensor_write(connector, HDC2080_ADDRESS, data_send, 2, 100);
	return retVal;

}


int8_t hdc2080_measure(uint8_t connector, float* temperature, uint8_t* humidity)
{
	uint8_t receive_data[4] = {1, 1, 1, 1};
	uint16_t temp_x,humi_x;
	int8_t retVal = 0;
	uint8_t send_data = HDC2080_TEMP_LOW_ADDRESS; //read all 4 bytes: temp_l, temp_h, hum_l, hum_h

	retVal = i2c_sensor_write(connector, HDC2080_ADDRESS, &send_data, 1, 100);
	if ( retVal != HAL_OK )
	{
		return retVal;
	}
	/* Read temperature and humidity */
	//HAL_I2C_Master_Receive(hi2c_x,HDC_1080_ADD<<1,receive_data,4,1000);
	retVal = i2c_sensor_read(connector, HDC2080_ADDRESS, receive_data, 4, 100);
	if ( retVal != HAL_OK )
	{
		return retVal;
	}


	/*
	send_data = HDC2080_TEMP_HIGH_ADDRESS; //read all 4 bytes: temp_l, temp_h, hum_l, hum_h
	retVal = i2c_sensor_write(HDC_connector, HDC2080_ADDRESS, &send_data, 1, 100);
	if ( retVal != HAL_OK )
	{
		return retVal;
	}
	retVal = i2c_sensor_read(HDC_connector, HDC2080_ADDRESS, &receive_data[1], 1, 100);
	if ( retVal != HAL_OK )
	{
		return retVal;
	}


	send_data = HDC2080_HUM_LOW_ADDRESS; //read all 4 bytes: temp_l, temp_h, hum_l, hum_h
	retVal = i2c_sensor_write(HDC_connector, HDC2080_ADDRESS, &send_data, 1, 100);
	if ( retVal != HAL_OK )
	{
		return retVal;
	}
	retVal = i2c_sensor_read(HDC_connector, HDC2080_ADDRESS, &receive_data[2], 1, 100);
	if ( retVal != HAL_OK )
	{
		return retVal;
	}

	send_data = HDC2080_HUM_HIGH_ADDRESS; //read all 4 bytes: temp_l, temp_h, hum_l, hum_h
	retVal = i2c_sensor_write(HDC_connector, HDC2080_ADDRESS, &send_data, 1, 100);
	if ( retVal != HAL_OK )
	{
		return retVal;
	}
	retVal = i2c_sensor_read(HDC_connector, HDC2080_ADDRESS, &receive_data[3], 1, 100);
	if ( retVal != HAL_OK )
	{
		return retVal;
	}

	*/

	temp_x =((receive_data[1]<<8)|receive_data[0]);
	humi_x =((receive_data[3]<<8)|receive_data[2]);

	*temperature=((temp_x/65536.0)*165.0)-40.5;
	*humidity=(uint8_t)((humi_x/65536.0)*100.0);

	return retVal;

}
