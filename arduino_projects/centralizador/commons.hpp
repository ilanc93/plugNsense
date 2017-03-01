#ifndef COMMONS_H
#define COMMONS_H
#include <Arduino.h>

typedef union {
  int16_t value;
  byte valueArray[2];
} myInt16_t;

typedef struct {
  byte sof;
  byte dest;
  byte orig;
  byte paylen;
  byte paydata[195];
  byte checksum;
} myFrame;

#endif
