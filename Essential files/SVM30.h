/*
 * SVM30.h
 *
 *  Created on: Nov 14, 2020
 *      Author: Ming
 */

#ifndef SVM30_H_
#define SVM30_H_

//Should be followed by a 15-second delay before calling getCO2eq(), getTVOC() or sendMeasureAirQuality()
void initAQ(uint8_t *buffer, I2C_HandleTypeDef periName);

float getTemp(uint8_t *buffer, I2C_HandleTypeDef periName);

float getRH(uint8_t *buffer, I2C_HandleTypeDef periName);

uint16_t getCO2eq(uint8_t *buffer, I2C_HandleTypeDef periName);

uint16_t getTVOC(uint8_t *buffer, I2C_HandleTypeDef periName);

//Regualrly called. for sensor correction only
void sendMeasureAirQuality(uint8_t *buffer, I2C_HandleTypeDef periName);

#endif /* SVM30_H_ */
