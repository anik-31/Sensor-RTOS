/*
 * dht11.c
 *
 *  Created on: May 5, 2025
 *      Author: anirudh
 */


#include "dht11.h"
#include "stm32f4xx_hal.h"

// DHT11 read buffer
static uint8_t data[5]; // 5 bytes of data

// Initialize the DHT11 sensor
HAL_StatusTypeDef DHT11_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Initialize GPIO pin as output
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

    return HAL_OK;
}

// Read temperature and humidity from DHT11
HAL_StatusTypeDef DHT11_Read(uint8_t *temperature, uint8_t *humidity) {
    uint8_t i, j;

    // Send start signal (pulling pin low for 18ms)
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(18);
    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);

    // Now configure the pin as input to receive data from DHT11
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

    // Wait for DHT11 response (it pulls the line low for 80us)
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET);

    // Wait for DHT11 to pull the line high
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET);

    // Wait for the line to go high again
    while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET);

    // Read the 40 bits of data from DHT11
    for (i = 0; i < 5; i++) {
        data[i] = 0;
        for (j = 0; j < 8; j++) {
            while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_RESET); // Wait for pin to go high
            HAL_Delay(1);
            if (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET) {
                data[i] |= (1 << (7 - j));
            }
            while (HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN) == GPIO_PIN_SET); // Wait for pin to go low
        }
    }

    // Check if checksum matches
    if (data[4] == (data[0] + data[1] + data[2] + data[3])) {
        *humidity = data[0];  // Humidity byte
        *temperature = data[2];  // Temperature byte
        return HAL_OK;
    } else {
        return HAL_ERROR;  // Checksum error
    }
}
