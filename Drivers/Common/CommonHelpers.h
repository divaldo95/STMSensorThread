/*
 * CommonHelpers.h
 *
 *  Created on: Sep 03, 2023
 *      Author: divaldo
 */

#ifndef COMMON_COMMONHELPERS_H_
#define COMMON_COMMONHELPERS_H_

#include <inttypes.h>
#include "../i2c_mux/i2c_mux.h"
#include "../ADS1110/ADS.h"
#include "../HDC/HDC2080.h"
#include "../SPS30/sps30.h"
#include "DataStructures.h"
#include "../StateMachine/StateMachine.h"

int16_t get_sensor_default_address(SensorType type)
{
	switch(type)
	{
		case ADS:
			return ADS_ADDRESS;
		case HDC2080:
			return HDC2080_ADDRESS;
		case SPS30:
			return SPS30_I2C_ADDRESS;
		default:
			return -1;
	}
}

SensorType get_sensor_type_by_address(int16_t addr)
{
	switch(addr)
	{
		case ADS_ADDRESS:
			return ADS;
		case HDC2080_ADDRESS:
			return HDC2080;
		case SPS30_I2C_ADDRESS:
			return SPS30;
		default:
			return None;
	}
}

void get_sensor_name_by_type(SensorType type, char *name, size_t len)
{
	switch(type)
	{
		case ADS:
			snprintf(name, len, "ADS1110");
			return;
		case HDC2080:
			snprintf(name, len, "HDC2080");
			return;
		case SPS30:
			snprintf(name, len, "SPS30");
			return;
		case None:
			snprintf(name, len, "None");
			return;
		default:
			snprintf(name, len, "Unknown");
			return;
	}
}

uint16_t init_sensor_on_bus_by_type(SensorType type, uint8_t connector)
{
	if(i2c_mux_select(connector) < 0)
	{
		printf("Failed to change i2c mux to %d\r\n", connector);
		return HAL_ERROR;
	}
	switch(type)
	{
		case ADS:
			//nothing to do here
			return HAL_OK;
		case HDC2080:
			if ( hdc2080_init(connector, HDC2080_TEMP_RES, HDC2080_HUM_RES) != HAL_OK )
			{
				printf("Could not init HDC2080\r\n");
			}
			return HAL_OK;
		case SPS30:
			SPS30_connector = connector;
			printf("SPS30_connector: %d\r\n", SPS30_connector);
			if(sps30_probe() == NO_ERROR)
			{
				HAL_Delay(200);
				int ret = sps30_start_measurement();
				if (ret < 0)
				{
					printf("Could not init SPS30 on connector %d\r\n", connector);
					return HAL_ERROR;
				}
			}
			else
			{
				printf("Could not probe SPS30 on connector %d\r\n", connector);
				return HAL_ERROR;
			}
			return HAL_OK;
		case None:
			//printf("Nothing to be done for a None device\r\n");
			return HAL_OK;
		default:
			printf("I don't know how to init Unknown device :(\r\n");
			return HAL_OK;
	}
}

uint16_t deinit_sensor_on_bus_by_type(SensorType type, uint8_t connector)
{
	if(i2c_mux_select(connector) < 0)
	{
		printf("Failed to change i2c mux to %d\r\n", connector);
		return HAL_ERROR;
	}
	switch(type)
	{
		case ADS:
			//nothing to do here
			return HAL_OK;
		case HDC2080:
			return HAL_OK;
		case SPS30:
			if(sps30_probe() == 0)
			{
				SPS30_connector = connector;
				int ret = sps30_stop_measurement();
				if (ret < 0)
				{
					printf("Could not stop SPS30 on connector %d\r\n", connector);
					return HAL_ERROR;
				}
			}
			else
			{
				printf("Could not probe SPS30 (deinit) on connector %d\r\n", connector);
				return HAL_ERROR;
			}
			return HAL_OK;
		case None:
			printf("Nothing to be done for a None device\r\n");
			return HAL_OK;
		default:
			printf("I don't know how to deinit Unknown device :(\r\n");
			return HAL_OK;
	}
}

#endif /* COMMON_COMMONHELPERS_H_ */
