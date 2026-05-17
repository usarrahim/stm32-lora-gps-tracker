#ifndef APP_APP_H
#define APP_APP_H

#include "main.h"
#include <stdbool.h>

typedef struct {
  bool imu_ok;
  bool gps_ok;
  bool lora_ok;
} App_Status_t;

bool App_Init(I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi, UART_HandleTypeDef *huart);
void App_Run(void);
const App_Status_t *App_GetStatus(void);

#endif
