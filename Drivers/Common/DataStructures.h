/*
 * DataStructures.h
 *
 *  Created on: Aug 31, 2023
 *      Author: divaldo
 */

#ifndef COMMON_DATASTRUCTURES_H_
#define COMMON_DATASTRUCTURES_H_

#include <inttypes.h>

#define meas_data_type_SPS30  			0x01
#define meas_data_type_SPS30_extended  	0x02
#define meas_data_type_ADS 				0x03
#define meas_data_type_HDC  			0x04

typedef enum {
	None = -1,
	ADS = 0,
	HDC2080 = 1,
	SPS30 = 2
} SensorType;

typedef enum {
	Off = 0,
	WarmUP = 1,
	Measurement = 2
} States;

struct sps30_measurement {
    float mc_1p0;
    float mc_2p5;
    float mc_4p0;
    float mc_10p0;
    float nc_0p5;
    float nc_1p0;
    float nc_2p5;
    float nc_4p0;
    float nc_10p0;
    float typical_particle_size;
    uint8_t connector;
};

struct ADS_data {
	float milivolts;
	int16_t raw;

	uint8_t connector;
};

struct HDC_data {
	float temperature;
	uint8_t humidity;
	uint8_t connector;
};

struct all_meas_data {
	struct sps30_measurement sps_meas;
	struct ADS_data ads_data;
	struct HDC_data hdc_data;
};

struct generalized_meas_data {
	SensorType sensor;
	uint8_t connector;
	void *data;
};

#endif /* COMMON_DATASTRUCTURES_H_ */
