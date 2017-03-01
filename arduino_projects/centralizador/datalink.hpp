#ifndef DATALINK_H
#define DATALINK_H
#include <Arduino.h>


boolean frame_parse();

byte frame_calculateChecksum( const byte paylen, const byte *paydata, const byte dest, const byte orig);

void frame_build();

void frame_send();


#endif
