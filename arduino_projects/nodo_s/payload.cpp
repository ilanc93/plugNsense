#include <Arduino.h>
#include "commons.hpp"
#include "payload.hpp"
#include "config.hpp"
#include "current.hpp"
#include <SoftwareSerial.h>
extern SoftwareSerial debugSerial;

extern myInt16_t timestamp;
extern uint32_t offsetTime;

extern byte i, j;
extern myFrame frame;

extern byte current_actualSample[ current_max_samples ];
extern myInt16_t current_samples[ current_sensors ][ current_max_samples ];

extern byte local_id;

extern void debug_printPaydata();

extern byte mac_self;
extern byte mac_random;

byte payload_parse(const byte paylen, const byte *paydata) {

  if ( paylen < 4 ) {
    return 0xFF;
  }

  byte type = paydata[0];

  //Se guarda global
  //A verificar el orden de los bytes.
  timestamp.valueArray[0] = (paydata)[1];
  timestamp.valueArray[1] = (paydata)[2];

  //Se guarda global
  local_id = (paydata)[3];

  return type;
}



void payload_parse_data_directive(const byte paylen, const byte *paydata) {
#if DEBUG
  debugSerial.println(F("DIRECTIVAS"));
#endif
  //Si es un byte es una directiva simple
  if (paylen == 1) {
#if DEBUG
    debugSerial.println(paylen, HEX);
    debugSerial.println(paydata[0], HEX);
#endif
    switch (paydata[0]) {
      case 0:
        turn_off();
#if DEBUG
        debugSerial.println(F("APAGAR"));
#endif
        break;

      case 1:
        turn_on();
#if DEBUG
        debugSerial.println(F("PRENDER"));
#endif
        break;
    }
  }
}


//Solo en nodo
//Antes de llamar a esta funcion ( o despues ) hay que resetear el current_actualSample correspondiente.
void payload_build_data_measurments(const byte local_id) {
  //Es un sensor de corriente.

  if ( local_id < 10 ) {

    byte aux[2 * current_max_samples + 1];

    aux[0] = current_actualSample[local_id];

    for ( i = 1; i <= 2 * current_actualSample[local_id]; i += 2 ) {
      aux[  i  ] = current_samples[local_id][(i - 1) / 2].valueArray[0];
      aux[i + 1] = current_samples[local_id][(i - 1) / 2].valueArray[1];
    }

    payload_build( TYPE_SEND_DATA, local_id, 2 * (current_actualSample[local_id]) + 1, aux);
#if DEBUG
    debugSerial.println("ENVIE LOS DATOS");
#endif
  }


}

//Nodo y Cerebro
void payload_build_data_ack() {
  //Aca hay que ver a quien se hace ack y cambiar el valor de timestamp
  payload_build( TYPE_ACK, 0, 0, NULL);
#if DEBUG
  debugSerial.println("ENVIO ACK AL CEREBRO");
#endif
}

//Solo en nodo
void payload_build_data_failures(const byte failureLen, const byte *failures) {
  timestamp.value = millis() - offsetTime;
  payload_build( TYPE_FAILURE, 0, failureLen, failures);
#if DEBUG
  debugSerial.println(F("ENVIO LAS FALLAS"));
#endif
}


bool parse_mac_id(const byte paylen, const byte *paydata) {
  if ( ( paylen == 2 ) && ( paydata[0] == mac_random) ) {
    mac_self = paydata[1];
    return true;
  } else {
    return false;
  }
}

//Solo en nodo
void payload_build_data_sendConfiguration(const byte ioDataLen, const byte *ioData) {
  payload_build( TYPE_SEND_CONFIGURATION, 0, ioDataLen, ioData);
}

void payload_build_recon() {
  payload_build( TYPE_RECON, 0, 1, &mac_random);
}

//Antes de llamar a esta funcion, timestamp tiene que tener el valor correspondiente.
void payload_build( const byte type, const byte local_id, const byte dataLen, const byte *data) {

  frame.paydata[0] = type;

  frame.paydata[1] = timestamp.valueArray[0];
  frame.paydata[2] = timestamp.valueArray[1];

  frame.paydata[3] = local_id;

  for ( i = 4; i < (4 + dataLen); i++) {
    frame.paydata[i] = data[i - 4];
  }

  frame.paylen = 4 + dataLen;

}
