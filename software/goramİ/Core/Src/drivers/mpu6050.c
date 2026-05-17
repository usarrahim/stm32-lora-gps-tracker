#include "drivers/mpu6050.h"

uint8_t MPU6050_Init(I2C_HandleTypeDef *hi2c)
{
  uint8_t id = 0;
  uint8_t wake = 0x00;

  if (HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, REG_WHO_AM_I, 1, &id, 1, 1000) != HAL_OK) {
    return 1;
  }
  if (id != 0x68U) {
    return 1;
  }
  if (HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, REG_PWR_MGMT_1, 1, &wake, 1, 1000) != HAL_OK) {
    return 1;
  }
  return 0;
}

void MPU6050_ReadAccel(I2C_HandleTypeDef *hi2c, MPU6050_t *data)
{
  uint8_t raw[6];

  if (HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, REG_ACCEL_XOUT_H, 1, raw, 6, 1000) != HAL_OK) {
    return;
  }

  data->Accel_X_RAW = (int16_t)((raw[0] << 8) | raw[1]);
  data->Accel_Y_RAW = (int16_t)((raw[2] << 8) | raw[3]);
  data->Accel_Z_RAW = (int16_t)((raw[4] << 8) | raw[5]);
  data->Ax = (float)data->Accel_X_RAW / 16384.0f;
  data->Ay = (float)data->Accel_Y_RAW / 16384.0f;
  data->Az = (float)data->Accel_Z_RAW / 16384.0f;
}
