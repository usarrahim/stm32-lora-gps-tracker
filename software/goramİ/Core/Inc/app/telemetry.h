#ifndef APP_TELEMETRY_H
#define APP_TELEMETRY_H

#include "drivers/gps.h"
#include "drivers/mpu6050.h"
#include <stdint.h>

int Telemetry_Build(char *buffer, uint16_t size, const GPS_Fix_t *gps, const MPU6050_t *imu);

#endif
