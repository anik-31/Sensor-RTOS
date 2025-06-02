/*
 * dht11.h
 *
 *  Created on: May 5, 2025
 *      Author: anirudh
 */

#ifndef DHT11_H
#define DHT11_H

#include "stm32f4xx_hal.h"

// DHT11 Pin definition
#define DHT11_PIN GPIO_PIN_1
#define DHT11_PORT GPIOA

// Function prototypes
HAL_StatusTypeDef DHT11_Init(void);
HAL_StatusTypeDef DHT11_Read(uint8_t *temperature, uint8_t *humidity);

#endif /* DHT11_H */
