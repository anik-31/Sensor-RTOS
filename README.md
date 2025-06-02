# Sensor-RTOS

This project demonstrates a simple RTOS-based system using FreeRTOS on an STM32F407 microcontroller. It integrates a DHT11 sensor for temperature and humidity measurements, sends the data via UART, and uses an LED to indicate system status. The code is structured into tasks with appropriate synchronization mechanisms like semaphores.

---

## Features
1. **DHT11 Sensor Integration**:
   - Reads temperature and humidity.
   - Uses a checksum to verify data integrity.

2. **UART Communication**:
   - Sends sensor data to a connected terminal.
   - Logs system messages for debugging.

3. **LED Indicator**:
   - Blinks to indicate system status.
   - Toggles on specific events (e.g., successful UART transmission).

4. **FreeRTOS Tasks**:
   - Three tasks: `DHT11Task`, `UARTTask`, and `LEDTask`.
   - Uses semaphores to synchronize between tasks.

---

## Task Overview
1. **DHT11Task**:
   - Reads data from the DHT11 sensor every 2 seconds.
   - Releases a semaphore when new data is available.

2. **UARTTask**:
   - Waits for the semaphore from `DHT11Task`.
   - Sends the temperature and humidity data via UART.

3. **LEDTask**:
   - Blinks the LED at a regular interval.

---

## Hardware Requirements
- **Microcontroller**: STM32F407VET6
- **Sensors**: DHT11 for temperature and humidity
- **UART Terminal**: USB-to-serial adapter (e.g., FTDI module)
- **LED**: Connected to GPIO pin `PB0`
- **Power Supply**: 5V DC

---

## Pin Configuration
| Peripheral | Pin Name | Pin Number |
|------------|----------|------------|
| DHT11      | GPIOA    | PA1        |
| UART TX    | GPIOA    | PA9        |
| UART RX    | GPIOA    | PA10       |
| LED        | GPIOB    | PB0        |

---

## Software Requirements
1. **IDE**: STM32CubeIDE
2. **RTOS**: FreeRTOS
3. **HAL Drivers**: Generated using STM32CubeMX
4. **UART Terminal Software**: Tera Term, PuTTY, or equivalent

---

## File Structure
├── Core
│ ├── Inc
│ │ ├── dht11.h # Header for DHT11 functions
│ │ └── main.h # Main header
│ ├── Src
│ │ ├── dht11.c # Implementation of DHT11 functions
│ │ ├── main.c # Main application
│ │ ├── stm32f4xx_hal_msp.c # HAL initialization
│ │ └── ... # Other HAL/RTOS files
├── FreeRTOS
│ ├── CMSIS
│ ├── Source
│ └── ...
└── README.md # Project documentation


---

## How to Run
1. **Hardware Setup**:
   - Connect the DHT11 sensor to `PA1`.
   - Connect an LED to `PB0`.
   - Connect a UART terminal to the UART1 pins (`PA9` and `PA10`).

2. **Software Setup**:
   - Open the project in STM32CubeIDE.
   - Build the project to generate the binary file.
   - Flash the binary file to the STM32F407 using an ST-Link programmer.

3. **Terminal Configuration**:
   - Baud Rate: 115200
   - Data Bits: 8
   - Parity: None
   - Stop Bits: 1
   - Flow Control: None

4. **Run the System**:
   - Open the UART terminal to view the temperature and humidity data.
   - Observe the LED blinking as per the task configuration.

---

## Example Output
**UART Terminal**:
System Initialized
Starting FreeRTOS scheduler...
DHT11 Task Started
UART Task Started
LED Task Started
Temp: 25°C, Hum: 60%
Temp: 26°C, Hum: 58%

**LED Behavior**:
- Blinks every 500ms.
- Toggles when `MAX_DATA_SENT` is reached.

---

## Customization
1. **Changing Sensor Read Interval**:
   - Modify the `osDelay(2000);` in `StartDHT11Task`.

2. **Adjusting UART Data Rate**:
   - Modify the UART baud rate in `MX_USART1_UART_Init`.

3. **Changing LED Blink Frequency**:
   - Modify the `osDelay(500);` in `StartLEDTask`.

---

## Known Issues
- **Checksum Failure**:
  - Ensure proper wiring and stable power supply for the DHT11 sensor.

- **UART Timeout**:
  - Increase the timeout in `HAL_UART_Transmit` if data loss occurs.

---

## Authors
- **Anirudh** - Project Developer

---


