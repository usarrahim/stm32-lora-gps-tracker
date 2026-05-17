#ifndef DRIVERS_E22_LORA_H
#define DRIVERS_E22_LORA_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

bool E22_Init(SPI_HandleTypeDef *hspi);
bool E22_Send(const uint8_t *payload, uint8_t length);

#endif
