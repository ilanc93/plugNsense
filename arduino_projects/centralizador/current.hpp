#ifndef CURRENT_H
#define CURRENT_H
#include "config.hpp"

void checkAndTakeCurrentMeasurements();

void takeSample(const byte actual_sensor);

int16_t readSample( const byte i );



#endif
