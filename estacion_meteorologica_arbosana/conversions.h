
#define UINT16_t_MAX  65536
#define INT16_t_MAX   UINT16_t_MAX/2
#define UINT8_t_MAX   256
#define INT8_t_MAX    UINT8_t_MAX/2
#define UINT10_t_MAX  1023

int16_t convertoFloatToInt16(float value, long max, long min) {
  float conversionFactor = (float) (INT16_t_MAX) / (float)(max - min);
  return (int16_t)(value * conversionFactor);
}

uint16_t convertoFloatToUInt16(float value, long max, long min = 0) {
  float conversionFactor = (float) (UINT16_t_MAX) / (float)(max - min);
  return (uint16_t)(value * conversionFactor);
}

uint8_t convertoFloatToUInt8(float value, long max, long min = 0) {
  float conversionFactor = (float) (UINT8_t_MAX) / (float)(max - min);
  return (uint8_t)(value * conversionFactor);
}

int8_t convertoFloatToInt8(float value, long max, long min = 0) {
  float conversionFactor = (float) (INT8_t_MAX) / (float)(max - min);
  return (int8_t)(value * conversionFactor);
}

uint8_t convertoInt10ToUInt8(int value, long max, long min = 0) {
  float conversionFactor = (float) (UINT8_t_MAX) / (float)(max - min);
  return (uint8_t)(value * conversionFactor);
}
