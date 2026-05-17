#include "app/telemetry.h"

#include <stdio.h>

static int32_t ToMilli(float value)
{
  if (value >= 0.0f) {
    return (int32_t)(value * 1000.0f + 0.5f);
  }
  return (int32_t)(value * 1000.0f - 0.5f);
}

int Telemetry_Build(char *buffer, uint16_t size, const GPS_Fix_t *gps, const MPU6050_t *imu)
{
  if (buffer == NULL || size == 0U || gps == NULL || imu == NULL) {
    return 0;
  }

  if (gps->valid) {
    int32_t lat = ToMilli(gps->latitude);
    int32_t lon = ToMilli(gps->longitude);
    int32_t alt = ToMilli(gps->altitude_m);
    int32_t ax = ToMilli(imu->Ax);
    int32_t ay = ToMilli(imu->Ay);
    int32_t az = ToMilli(imu->Az);

    return snprintf(buffer, size,
                    "T,%ld,%ld,%ld,%u,%u,%ld,%ld,%ld\r\n",
                    (long)lat, (long)lon, (long)alt,
                    (unsigned)gps->fix, (unsigned)gps->satellites,
                    (long)ax, (long)ay, (long)az);
  }

  return snprintf(buffer, size,
                  "T,NA,NA,NA,0,%u,%ld,%ld,%ld\r\n",
                  (unsigned)gps->satellites,
                  (long)ToMilli(imu->Ax),
                  (long)ToMilli(imu->Ay),
                  (long)ToMilli(imu->Az));
}
