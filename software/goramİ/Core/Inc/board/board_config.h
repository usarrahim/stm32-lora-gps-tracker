#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "main.h"

#define GPS_UART_BAUD           9600U
#define TELEMETRY_INTERVAL_MS   1000U

#define LORA_NRST_Pin           GPIO_PIN_8
#define LORA_NRST_GPIO_Port     GPIOA
#define LORA_BUSY_Pin           GPIO_PIN_12
#define LORA_BUSY_GPIO_Port     GPIOA

#define E22_CHANNEL             23U
#define E22_NETID               0x00U
#define E22_ADDR_H              0x00U
#define E22_ADDR_L              0x01U
#define E22_TX_TIMEOUT_MS       3000U
#define E22_BUSY_TIMEOUT_MS     500U

#endif
