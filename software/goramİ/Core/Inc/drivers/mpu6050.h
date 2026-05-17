#ifndef DRIVERS_MPU6050_H
#define DRIVERS_MPU6050_H

#include "main.h"
#include <stdint.h>

#define MPU6050_ADDR        0xD0U
#define REG_WHO_AM_I        0x75U
#define REG_PWR_MGMT_1      0x6BU
#define REG_ACCEL_XOUT_H    0x3BU

typedef struct {
  int16_t Accel_X_RAW;
  int16_t Accel_Y_RAW;
  int16_t Accel_Z_RAW;
  float Ax;
  float Ay;
  float Az;
} MPU6050_t;

uint8_t MPU6050_Init(I2C_HandleTypeDef *hi2c);
void MPU6050_ReadAccel(I2C_HandleTypeDef *hi2c, MPU6050_t *data);

#endif
