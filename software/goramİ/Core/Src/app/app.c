#include "app/app.h"
#include "app/telemetry.h"
#include "board/board.h"
#include "board/board_config.h"
#include "drivers/e22_lora.h"
#include "drivers/gps.h"
#include "drivers/mpu6050.h"

#include <string.h>

#define TX_PAYLOAD_MAX   128U

static I2C_HandleTypeDef *app_i2c;
static SPI_HandleTypeDef *app_spi;
static UART_HandleTypeDef *app_uart;
static App_Status_t app_status;
static MPU6050_t imu_data;
static char tx_payload[TX_PAYLOAD_MAX];
static uint32_t last_tx_tick;

bool App_Init(I2C_HandleTypeDef *hi2c, SPI_HandleTypeDef *hspi, UART_HandleTypeDef *huart)
{
  memset(&app_status, 0, sizeof(app_status));
  memset(&imu_data, 0, sizeof(imu_data));
  last_tx_tick = HAL_GetTick();

  app_i2c = hi2c;
  app_spi = hspi;
  app_uart = huart;

  Board_LoRaPinsInit();

  app_status.imu_ok = (MPU6050_Init(app_i2c) == 0U);
  GPS_Init(app_uart);
  app_status.gps_ok = true;
  app_status.lora_ok = E22_Init(app_spi);

  return app_status.imu_ok && app_status.lora_ok;
}

void App_Run(void)
{
  int payload_len;

  if ((HAL_GetTick() - last_tx_tick) < TELEMETRY_INTERVAL_MS) {
    return;
  }
  last_tx_tick = HAL_GetTick();

  GPS_Process();

  if (app_status.imu_ok) {
    MPU6050_ReadAccel(app_i2c, &imu_data);
  }

  payload_len = Telemetry_Build(tx_payload, TX_PAYLOAD_MAX, GPS_GetFix(), &imu_data);
  if (payload_len <= 0) {
    return;
  }
  if (payload_len > 240) {
    payload_len = 240;
  }

  if (app_status.lora_ok) {
    app_status.lora_ok = E22_Send((const uint8_t *)tx_payload, (uint8_t)payload_len);
  }
}

const App_Status_t *App_GetStatus(void)
{
  return &app_status;
}
