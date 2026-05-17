#include "drivers/gps.h"
#include "board/board_config.h"

#include <stdlib.h>
#include <string.h>

#define GPS_RX_BUF_SIZE   256U
#define GPS_LINE_MAX      96U

static UART_HandleTypeDef *gps_uart;
static GPS_Fix_t gps_fix;
static uint8_t rx_byte;
static char line_buf[GPS_LINE_MAX];
static uint16_t line_len;

static float NMEA_ToDegrees(const char *field)
{
  float val = (float)atof(field);
  int deg = (int)(val / 100.0f);
  float minutes = val - (float)(deg * 100);
  return (float)deg + (minutes / 60.0f);
}

static void GPS_ParseLine(char *line)
{
  char *star = strchr(line, '*');
  if (star != NULL) {
    *star = '\0';
  }

  if (strncmp(line, "$GPGGA", 6) != 0 && strncmp(line, "$GNGGA", 6) != 0) {
    return;
  }

  char *tok = strtok(line, ",");
  uint8_t field = 0;
  char lat[16] = {0};
  char lon[16] = {0};
  char lat_h = 'N';
  char lon_h = 'E';

  while (tok != NULL) {
    switch (field) {
      case 2:
        strncpy(lat, tok, sizeof(lat) - 1U);
        break;
      case 3:
        lat_h = tok[0];
        break;
      case 4:
        strncpy(lon, tok, sizeof(lon) - 1U);
        break;
      case 5:
        lon_h = tok[0];
        break;
      case 6:
        gps_fix.fix = (uint8_t)atoi(tok);
        break;
      case 7:
        gps_fix.satellites = (uint8_t)atoi(tok);
        break;
      case 9:
        gps_fix.altitude_m = (float)atof(tok);
        break;
      default:
        break;
    }
    tok = strtok(NULL, ",");
    field++;
  }

  if (lat[0] == '\0' || lon[0] == '\0' || gps_fix.fix == 0U) {
    gps_fix.valid = false;
    return;
  }

  gps_fix.latitude = NMEA_ToDegrees(lat);
  gps_fix.longitude = NMEA_ToDegrees(lon);
  if (lat_h == 'S' || lat_h == 's') {
    gps_fix.latitude = -gps_fix.latitude;
  }
  if (lon_h == 'W' || lon_h == 'w') {
    gps_fix.longitude = -gps_fix.longitude;
  }
  gps_fix.valid = true;
}

static void GPS_PushByte(uint8_t byte)
{
  if (byte == '\n' || byte == '\r') {
    if (line_len > 0U) {
      line_buf[line_len] = '\0';
      GPS_ParseLine(line_buf);
      line_len = 0U;
    }
    return;
  }

  if (line_len < (GPS_LINE_MAX - 1U)) {
    line_buf[line_len++] = (char)byte;
  } else {
    line_len = 0U;
  }
}

void GPS_Init(UART_HandleTypeDef *huart)
{
  gps_uart = huart;
  memset(&gps_fix, 0, sizeof(gps_fix));
  line_len = 0U;

  huart->Init.BaudRate = GPS_UART_BAUD;
  if (HAL_UART_Init(huart) != HAL_OK) {
    return;
  }

  HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  HAL_UART_Receive_IT(huart, &rx_byte, 1);
}

void GPS_Process(void)
{
  (void)gps_fix;
}

const GPS_Fix_t *GPS_GetFix(void)
{
  return &gps_fix;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance != USART1 || gps_uart == NULL) {
    return;
  }

  GPS_PushByte(rx_byte);
  HAL_UART_Receive_IT(gps_uart, &rx_byte, 1);
}
