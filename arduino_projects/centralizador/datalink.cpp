#include "datalink.hpp"
#include "commons.hpp"
#include "config.hpp"
#include "payload.hpp"
#include <SoftwareSerial.h>

extern byte i, j;
extern myFrame frame;

extern byte link_buffer[MAC_BUFFLEN];

extern byte mac_self;
extern byte mac_node;

extern SoftwareSerial debugSerial;

//Recibe un buffer con un start of frame valido.
boolean frame_parse() {

  //frame.sof     =   link_buffer[0];

  frame.dest    =   link_buffer[4];

  frame.orig    =   link_buffer[5];
  frame.paylen  =   link_buffer[6];

  frame.checksum = frame_calculateChecksum( frame.paylen, &(link_buffer[7]), frame.dest, frame.orig );
  if ( frame.checksum != link_buffer[ frame.paylen + 7 ] ) {
    debugSerial.println("checksum false");
    return false;
  }

  for ( i = 0; i < frame.paylen; i++ ) {
    frame.paydata[ i ] = link_buffer[ 7 + i ];
  }

  return true;
}

//Checksum con cantidad de 1s y overflow.
byte frame_calculateChecksum( const byte paylen, const byte *paydata, const byte dest, const byte orig) {
  byte check_sum = 0;
  for ( i = 0; i < paylen; i++ ) {
    byte byte_act = paydata[i];
    for ( j = 0; j <= 7; j++ ) {
      check_sum += (byte_act >> j) & 0x01;
    }
  }

  for ( j = 0; j <= 7; j++ ) {
    check_sum += (dest >> j) & 0x01;
  }

  for ( j = 0; j <= 7; j++ ) {
    check_sum += (orig >> j) & 0x01;
  }

  for ( j = 0; j <= 7; j++ ) {
    check_sum += (paylen >> j) & 0x01;
  }

  return check_sum;
}


void frame_build() {
#if DEBUG
  debugSerial.println(F("ENTRAMOS A FRAME BUILD"));
#endif
  //frame.sof = MAC_SOF;
  frame.dest = mac_node;
  frame.orig = mac_self;
  /*
    frame.paylen = paylen;
    for ( i = 0; i < paylen; i++ ){
      frame.paydata[i] = paydata[i];
    }
  */
  frame.checksum = frame_calculateChecksum(frame.paylen, frame.paydata, frame.dest, frame.orig);

}



void frame_send() {
#if DEBUG
  debugSerial.println(F("ENTRAMOS A FRAME SEND"));
#endif
  Serial.write(MAC_SOF);
  Serial.write(MAC_SOF);
  Serial.write(MAC_SOF);
  Serial.write(MAC_SOF);

  Serial.write(frame.dest);
  Serial.write(frame.orig);
  Serial.write(frame.paylen);
  for ( byte k = 0; k < frame.paylen; k++ ) {
    Serial.write(frame.paydata[k]);
  }
  Serial.write(frame.checksum);

}


