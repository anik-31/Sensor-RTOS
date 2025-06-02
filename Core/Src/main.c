/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "dht11.h"
#include "string.h"
#include <stdio.h>

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
osThreadId_t DHT11TaskHandle;
osThreadId_t UARTTaskHandle;
osThreadId_t LEDTaskHandle;
osSemaphoreId_t dataReadySemaphoreHandle;

/* Definitions for tasks */
const osThreadAttr_t DHT11Task_attributes = {
  .name = "DHT11Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

const osThreadAttr_t UARTTask_attributes = {
  .name = "UARTTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t LEDTask_attributes = {
  .name = "LEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private variables ---------------------------------------------------------*/
uint8_t dht11_data[5];
uint8_t data_sent_count = 0;
#define MAX_DATA_SENT 10

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
void StartDHT11Task(void *argument);
void StartUARTTask(void *argument);
void StartLEDTask(void *argument);

/* USER CODE BEGIN PFP */
/* Redirect printf to UART */
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
    return len;
}
/* USER CODE END PFP */

/* Main function -------------------------------------------------------------*/
int main(void)
{
  /* Reset peripherals and initialize Flash & Systick */
  HAL_Init();

  /* Configure system clock */
  SystemClock_Config();

  /* Initialize peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  printf("\r\nSystem Initialized\r\n");
  printf("Starting FreeRTOS scheduler...\r\n");
  /* USER CODE END 2 */

  /* Initialize RTOS kernel */
  osKernelInitialize();

  /* Create semaphore */
  dataReadySemaphoreHandle = osSemaphoreNew(1, 0, NULL);

  /* Create tasks */
  DHT11TaskHandle = osThreadNew(StartDHT11Task, NULL, &DHT11Task_attributes);
  UARTTaskHandle = osThreadNew(StartUARTTask, NULL, &UARTTask_attributes);
  LEDTaskHandle = osThreadNew(StartLEDTask, NULL, &LEDTask_attributes);

  /* Start scheduler */
  osKernelStart();

  /* Infinite loop */
  while (1)
  {
  }
}

/* Task Definitions ----------------------------------------------------------*/
void StartDHT11Task(void *argument)
{
  uint8_t temperature, humidity;
  /* USER CODE BEGIN StartDHT11Task */
  printf("DHT11 Task Started\r\n");
  /* USER CODE END StartDHT11Task */

  for (;;)
  {
    if (DHT11_Read(&temperature, &humidity) == HAL_OK)
    {
      dht11_data[0] = temperature;
      dht11_data[1] = humidity;
      osSemaphoreRelease(dataReadySemaphoreHandle);
      /* USER CODE BEGIN DHT11_Read_Success */
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // Visual indicator
      osDelay(10);
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
      /* USER CODE END DHT11_Read_Success */
    }
    else
    {
      /* USER CODE BEGIN DHT11_Read_Fail */
      printf("DHT11 Read Failed\r\n");
      /* USER CODE END DHT11_Read_Fail */
    }
    osDelay(2000); // Increased delay to 2 seconds
  }
}

void StartUARTTask(void *argument)
{
  char uart_buffer[64];
  /* USER CODE BEGIN StartUARTTask */
  printf("UART Task Started\r\n");
  /* USER CODE END StartUARTTask */

  for (;;)
  {
    if (osSemaphoreAcquire(dataReadySemaphoreHandle, 1000) == osOK) // 1s timeout
    {
      int len = snprintf(uart_buffer, sizeof(uart_buffer),
                      "Temp: %dC, Hum: %d%%\r\n", // Added \r for Windows terminals
                      dht11_data[0], dht11_data[1]);

      if (HAL_UART_Transmit(&huart1, (uint8_t*)uart_buffer, len, 1000) != HAL_OK)
      {
        /* USER CODE BEGIN UART_Error */
        printf("UART Transmit Failed\r\n");
        /* USER CODE END UART_Error */
      }

      data_sent_count++;
      if (data_sent_count >= MAX_DATA_SENT)
      {
        data_sent_count = 0;
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
      }
    }
    else
    {
      /* USER CODE BEGIN Semaphore_Timeout */
      printf("Waiting for DHT11 data...\r\n");
      /* USER CODE END Semaphore_Timeout */
    }
  }
}

void StartLEDTask(void *argument)
{
  /* USER CODE BEGIN StartLEDTask */
  printf("LED Task Started\r\n");
  /* USER CODE END StartLEDTask */

  for (;;)
  {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Active LED blinking
    osDelay(500);
  }
}

/* Peripheral Initialization -------------------------------------------------*/
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* Error Handler -------------------------------------------------------------*/
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Fast blink on error
    osDelay(100);
  }
  /* USER CODE END Error_Handler_Debug */
}

/* System Clock Configuration ------------------------------------------------*/
void SystemClock_Config(void)
{
  // Add your clock configuration code
}

/* USER CODE BEGIN 4 */
// Additional user code can be added here
/* USER CODE END 4 */
