#include "drivers/e22_lora.h"
#include "board/board_config.h"

#include <string.h>

#define E22_CMD_WRITE       0xC0U
#define E22_CMD_READ        0xC1U
#define E22_CMD_TX          0xC2U

#define E22_REG_ADDH        0x00U
#define E22_REG_ADDL        0x01U
#define E22_REG_NETID       0x02U
#define E22_REG_REG0        0x03U
#define E22_REG_REG1        0x04U
#define E22_REG_REG2        0x05U
#define E22_REG_REG3        0x06U

static SPI_HandleTypeDef *lora_spi;

static void E22_NssLow(void)
{
  HAL_GPIO_WritePin(LORA_NSS_GPIO_Port, LORA_NSS_Pin, GPIO_PIN_RESET);
}

static void E22_NssHigh(void)
{
  HAL_GPIO_WritePin(LORA_NSS_GPIO_Port, LORA_NSS_Pin, GPIO_PIN_SET);
}

static bool E22_WaitBusy(uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();
  while (HAL_GPIO_ReadPin(LORA_BUSY_GPIO_Port, LORA_BUSY_Pin) == GPIO_PIN_SET) {
    if ((HAL_GetTick() - start) > timeout_ms) {
      return false;
    }
  }
  return true;
}

static void E22_Reset(void)
{
  HAL_GPIO_WritePin(LORA_NRST_GPIO_Port, LORA_NRST_Pin, GPIO_PIN_RESET);
  HAL_Delay(20);
  HAL_GPIO_WritePin(LORA_NRST_GPIO_Port, LORA_NRST_Pin, GPIO_PIN_SET);
  HAL_Delay(120);
}

static bool E22_WriteRegs(uint8_t reg, const uint8_t *data, uint8_t len)
{
  uint8_t frame[16];

  if ((uint16_t)len + 3U > sizeof(frame)) {
    return false;
  }

  frame[0] = E22_CMD_WRITE;
  frame[1] = reg;
  frame[2] = len;
  memcpy(&frame[3], data, len);

  if (!E22_WaitBusy(E22_BUSY_TIMEOUT_MS)) {
    return false;
  }

  E22_NssLow();
  HAL_StatusTypeDef st = HAL_SPI_Transmit(lora_spi, frame, (uint16_t)(len + 3U), 200);
  E22_NssHigh();
  return st == HAL_OK;
}

static bool E22_Configure(void)
{
  const uint8_t addh = E22_ADDR_H;
  const uint8_t addl = E22_ADDR_L;
  const uint8_t netid = E22_NETID;
  const uint8_t reg0 = 0x1AU;
  const uint8_t reg1 = 0x27U;
  const uint8_t reg2 = E22_CHANNEL;
  const uint8_t reg3 = 0x00U;

  if (!E22_WriteRegs(E22_REG_ADDH, &addh, 1)) return false;
  if (!E22_WriteRegs(E22_REG_ADDL, &addl, 1)) return false;
  if (!E22_WriteRegs(E22_REG_NETID, &netid, 1)) return false;
  if (!E22_WriteRegs(E22_REG_REG0, &reg0, 1)) return false;
  if (!E22_WriteRegs(E22_REG_REG1, &reg1, 1)) return false;
  if (!E22_WriteRegs(E22_REG_REG2, &reg2, 1)) return false;
  if (!E22_WriteRegs(E22_REG_REG3, &reg3, 1)) return false;
  return true;
}

bool E22_Init(SPI_HandleTypeDef *hspi)
{
  lora_spi = hspi;
  E22_NssHigh();
  E22_Reset();
  return E22_Configure();
}

bool E22_Send(const uint8_t *payload, uint8_t length)
{
  uint8_t header[2];

  if (payload == NULL || length == 0U) {
    return false;
  }

  if (!E22_WaitBusy(E22_BUSY_TIMEOUT_MS)) {
    return false;
  }

  header[0] = E22_CMD_TX;
  header[1] = length;

  E22_NssLow();
  if (HAL_SPI_Transmit(lora_spi, header, 2U, 200) != HAL_OK) {
    E22_NssHigh();
    return false;
  }
  if (HAL_SPI_Transmit(lora_spi, (uint8_t *)payload, length, 2000) != HAL_OK) {
    E22_NssHigh();
    return false;
  }
  E22_NssHigh();

  return E22_WaitBusy(E22_TX_TIMEOUT_MS);
}
