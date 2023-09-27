#include "i2c_mux.h"

uint8_t last_selected_bus = 10;



int8_t i2c_mux_select(uint8_t connector)
{
	if(connector > 4 )
	{
		return -2;
	}

	const uint8_t bus_connector_lut[5] = {0x05, 0x04, 0x06, 0x07, 0x00};
	uint8_t bus = bus_connector_lut[connector];

	if(bus != last_selected_bus)
	{
		if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(i2c_mux_address << 1), &bus, 1, 100) != HAL_OK)
		{
			last_selected_bus = 10;
			return -1;
		}
		last_selected_bus = bus;

	}

	return 0;
}


int8_t i2c_sensor_read(uint8_t connector, uint8_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	if(i2c_mux_select(connector) < 0)
	{
			return -1;
	}

	if(HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(DevAddress << 1), pData, Size, Timeout) != HAL_OK)
	{
			return -2;
	}

	return 0;
}


int8_t i2c_sensor_write(uint8_t connector, uint8_t DevAddress, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	if(i2c_mux_select(connector) < 0)
	{
			return -1;
	}

	if(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(DevAddress << 1), (uint8_t*)pData, Size, Timeout) != HAL_OK)
	{
			return -2;
	}
	return 0;
}

SensorType i2c_sensor_scan(uint8_t connector)
{
	if(i2c_mux_select(connector) < 0)
	{
		printf("Failed to change i2c mux to %d\n", connector);
		return -1;
	}
	printf("Scanning I2C bus:\r\n");
	HAL_StatusTypeDef result;
	uint8_t i;
	int16_t addr = -1;
	for (i=1; i<128; i++)
	{
	  /*
	   * the HAL wants a left aligned i2c address
	   * &hi2c1 is the handle
	   * (uint16_t)(i<<1) is the i2c address left aligned
	   * retries 2
	   * timeout 2
	   */
		int skip = 0;
		for (int j = 0; j < (sizeof(i2c_skip_addresses) / sizeof(uint8_t)); j++)
		{
			if (i2c_skip_addresses[j] == i)
			{
				skip = 1;
			}
		}
		if (skip)
		{
			continue;
		}
	  result = HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(i<<1), 2, 2);
	  if (result != HAL_OK) // HAL_ERROR or HAL_BUSY or HAL_TIMEOUT
	  {
		  printf("."); // No ACK received at that address
	  }
	  if (result == HAL_OK)
	  {
		  if (addr > 0)
		  {
			  printf("Multiple devices connected to connector %d: 0x%X, 0x%X\n", connector, addr, i);
		  }
		  else
		  {
			  addr = i;
			  printf("0x%X", i); // Received an ACK at that address
		  }
	  }
	}
	return get_sensor_type_by_address(addr);
}

void enable_5v()
{
	HAL_GPIO_WritePin(EN_5V_GPIO_Port, EN_5V_Pin, 1);
	HAL_Delay(100);
}

void disable_5v()
{
	HAL_GPIO_WritePin(EN_5V_GPIO_Port, EN_5V_Pin, 0);
}
