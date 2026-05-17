#ifndef DRIVERS_GPS_H
#define DRIVERS_GPS_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  float latitude;
  float longitude;
  float altitude_m;
  uint8_t fix;
  uint8_t satellites;
  bool valid;
} GPS_Fix_t;

void GPS_Init(UART_HandleTypeDef *huart);
void GPS_Process(void);
const GPS_Fix_t *GPS_GetFix(void);

#endif
