/*
 * libSVM30.c
 *
 *  Created on: Nov 14, 2020
 *      Author: Ming
 */

#include "main.h"

#include <stdio.h>
#include <string.h>

#include "i2c.h"


void initAQ(uint8_t *buffer, I2C_HandleTypeDef periName){

	HAL_StatusTypeDef ret;
	buffer[0] = 0x20;	//2) load I2C buffer
	buffer[1] = 0x03;	//Hex code -> Init_air_quality -> P10

	ret = HAL_I2C_Master_Transmit(&periName, 0x58 << 1, buffer, 2, HAL_MAX_DELAY);	//3) I2C transmit
	if (ret != HAL_OK){
	 strcpy((char *)buffer, "Error initAQ\r\n");
	}

	return;
}

float getTemp(uint8_t *buffer, I2C_HandleTypeDef periName){

	HAL_StatusTypeDef ret;
	uint16_t valT; 					//raw temperature
	float T; 						//readable temperature

	buffer[0] = 0x58;				//2) load I2C buffer
	buffer[1] = 0xE0;				//Command -> read RH first

	ret = HAL_I2C_Master_Transmit(&periName, 0x70 << 1, buffer, 2, HAL_MAX_DELAY);		//3) I2C transmit
	if (ret != HAL_OK){
	 strcpy((char *)buffer, "Error Tx\r\n");
	}else{
		HAL_Delay(100); //wait
		ret = HAL_I2C_Master_Receive(&periName, 0x70 << 1, buffer, 6, HAL_MAX_DELAY);	//4) I2C receive
		if (ret != HAL_OK){
			strcpy((char *)buffer, "Error Rx\r\n");
		}else{
			valT = ((int16_t)buffer[3] << 8) | buffer[4];	//5) read from buffer
			T = (float) (-45.68) + 175.7 * ((float)valT / 65536);	//6) calculation
		}
	 }

	return T;
}

float getRH(uint8_t *buffer, I2C_HandleTypeDef periName){

	HAL_StatusTypeDef ret;
	uint16_t valT;
	uint16_t valRH;
	float RH;

	buffer[0] = 0x58;	//2) load I2C buffer
	buffer[1] = 0xE0;	//Command -> read RH first

	ret = HAL_I2C_Master_Transmit(&periName, 0x70 << 1, buffer, 2, HAL_MAX_DELAY);	//3) I2C transmit
	if (ret != HAL_OK){
	 strcpy((char *)buffer, "Error Tx\r\n");
	}else{
		HAL_Delay(100); //wait
		ret = HAL_I2C_Master_Receive(&periName, 0x70 << 1, buffer, 6, HAL_MAX_DELAY);	//4) I2C receive
		if (ret != HAL_OK){
			strcpy((char *)buffer, "Error Rx\r\n");
		}else{
			valT = ((int16_t)buffer[3] << 8) | buffer[4];
			valRH = ((int16_t)buffer[0] << 8) | buffer[1];	//5) read from buffer
			RH = (float) (103.7 - 3.2 * ((float)valT / 65536)) * ((float)valRH / 65536);	//6) calculation
		}
	 }

	return RH;
}

uint16_t getCO2eq(uint8_t *buffer, I2C_HandleTypeDef periName){

	//sends a single Measure_air_quality, and then read from buffer

	HAL_StatusTypeDef ret;
	uint16_t valCO2;

	buffer[0] = 0x20;	//2) load I2C buffer
	buffer[1] = 0x08;	//Command -> Measure_air_quality

	ret = HAL_I2C_Master_Transmit(&periName, 0x58 << 1, buffer, 2, HAL_MAX_DELAY);	//3) I2C transmit
	if (ret != HAL_OK){
	 strcpy((char *)buffer, "Error Tx\r\n");
	}else{
		HAL_Delay(100); //wait
		ret = HAL_I2C_Master_Receive(&periName, 0x58 << 1, buffer, 6, HAL_MAX_DELAY);	//4) I2C receive
		if (ret != HAL_OK){
			strcpy((char *)buffer, "Error Rx\r\n");
		}else{
			valCO2 = ((int16_t)buffer[0] << 8) | buffer[1];	//5) read from buffer
			//6) no calculation
		}
	 }

	return valCO2;
}

uint16_t getTVOC(uint8_t *buffer, I2C_HandleTypeDef periName){

	//sends a single Measure_air_quality, and then read from buffer

	HAL_StatusTypeDef ret;
	uint16_t valTVOC;

	buffer[0] = 0x20;	//2) load I2C buffer
	buffer[1] = 0x08;	//Command -> Measure_air_quality

	ret = HAL_I2C_Master_Transmit(&periName, 0x58 << 1, buffer, 2, HAL_MAX_DELAY);	//3) I2C transmit
	if (ret != HAL_OK){
	 strcpy((char *)buffer, "Error Tx\r\n");
	}else{
		HAL_Delay(100); //wait
		ret = HAL_I2C_Master_Receive(&periName, 0x58 << 1, buffer, 6, HAL_MAX_DELAY);	//4) I2C receive
		if (ret != HAL_OK){
			strcpy((char *)buffer, "Error Rx\r\n");
		}else{
			valTVOC = ((int16_t)buffer[3] << 8) | buffer[4];	//5) read from buffer
			//6) no calculation
		}
	 }

	return valTVOC;
}

void sendMeasureAirQuality(uint8_t *buffer, I2C_HandleTypeDef periName){

	//for the req. of regular command of Measure_air_quality only -> P11

	HAL_StatusTypeDef ret;
	buffer[0] = 0x20;	//Hex code -> Measure_air_quality -> P10
	buffer[1] = 0x08;

	ret = HAL_I2C_Master_Transmit(&periName, 0x58 << 1, buffer, 2, 500);	//3) I2C transmit and check status
	if (ret != HAL_OK){
	 strcpy((char *)buffer, "Error sendMAQ\r\n");
	}


	return;
}




