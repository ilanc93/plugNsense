#include "current.hpp"
#include <SoftwareSerial.h>
extern SoftwareSerial debugSerial;

extern unsigned long current_prevM;
extern myInt16_t current_samples[ current_sensors ][ current_max_samples ];
extern byte current_actualSample[ current_sensors ];
extern unsigned long current_firstSampleTime[ current_sensors ];
extern byte i;
extern byte j;


void checkAndTakeCurrentMeasurements() {
  if ( (millis() - current_prevM ) >= current_sampling_period ) {
    for ( i = 0; i < current_sensors; i++ ) {
      if ( current_actualSample[i] == 0 ) {
        current_firstSampleTime[i] = millis();
      }

      takeSample(i);
      current_actualSample[i]++;


      if ( current_actualSample[i] == current_max_samples ) {
        current_actualSample[i] = 0;
      }
    }

    current_prevM = millis();

#if DEBUG
    for ( i = 0; i < current_sensors; i++ ) {
      for ( j = 0 ; j < current_actualSample[i]; j++ ) {
        debugSerial.print(current_samples[i][j].value, DEC);
        debugSerial.print(F(" | "));
      }
      debugSerial.println(F(""));
    }
    debugSerial.println(F(""));
#endif
  }

}

void takeSample(const byte actual_sensor) {
  current_samples[ actual_sensor ][ current_actualSample[actual_sensor] ].value = readSample( actual_sensor );
}


int16_t readSample( const byte i ) {
  return analogRead( current_pins[i] );
}




