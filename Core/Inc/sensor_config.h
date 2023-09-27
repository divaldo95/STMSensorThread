/*
 * sensor_config.h
 *
 *  Created on: 2022. okt. 14.
 *      Author: zbiro
 */

#ifndef INC_SENSOR_CONFIG_H_
#define INC_SENSOR_CONFIG_H_

#define AUTO_SCAN_SENSORS

#ifndef AUTO_SCAN_SENSORS
	#define SPS30_connector 0
	#define HDC_connector 	1
	#define ADS_connector	2
#else
	static uint8_t SPS30_connector = 0;
#endif

//#define emulate_sps30 1
//#define emulate_hdc2080 1
//#define emulate_ads 1

#define ADS_SAMPLES_PER_SEC SPS_15
#define ADS_GAIN			GAIN1


#define HDC2080_TEMP_RES	Temperature_Resolution_14_bit
#define HDC2080_HUM_RES		Humidity_Resolution_14_bit

#endif /* INC_SENSOR_CONFIG_H_ */
