#ifndef CURRENT_H
#define CURRENT_H
#include "config.hpp"

#define RELAY_ON  LOW
#define RELAY_OFF HIGH

void checkAndTakeCurrentMeasurements();

void takeSample(const byte actual_sensor);

int16_t readSample( const byte i );

void current_init();

void turn_on();

void turn_off();

#endif
