#include <Arduino.h>
#include "commons.hpp"
#include "payload.hpp"
#include "config.hpp"
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





void payload_parse_data_measurements(const byte *paydata) {
  byte meas_amnt = paydata[0];
#if DEBUG
  //debugSerial.println(F("MEASUREMENTS"));
  //debugSerial.println(meas_amnt, DEC);
#endif
  //debugSerial.println(F("MEASUREMENTS"));
  //debugSerial.println(meas_amnt, DEC);
  int16_t medidas[current_max_samples];
  myInt16_t meas;
  char u = 0;
  for (i = 1; i <= 2 * meas_amnt; i += 2) {
    meas.valueArray[0] = paydata[i];
    meas.valueArray[1] = paydata[i + 1];
    medidas[ u ] = meas.value;
    //debugSerial.print(meas.value, DEC);
    //debugSerial.print(F(" - "));
    u++;
#if DEBUG
    //debugSerial.print(meas.value, DEC);
    //debugSerial.print(F(" | "));
#endif
    //Envia cada medida a la pi. Se asume que el identificador del sensor y del nodo se saben del polling (o se envia antes)
    //pi_send_measurment(meas);
  }
  sendResponseMeasurements(medidas, meas_amnt);
    

#if DEBUG
  //debugSerial.println(F(""));
#endif
}

void payload_parse_data_directive(const byte paylen, const byte *paydata, const byte local_id) {
#if DEBUG
  debugSerial.println(F("DIRECTIVASSS"));
#endif
  //Si es un byte es una directiva simple
  if (paylen == 1) {
    switch (paydata[0]) {
      case 0:
        //turn_off(local_id);
        payload_build_data_ack();
#if DEBUG
        debugSerial.println(F("APAGAR"));
#endif
        break;

      case 1:
        //turn_on(local_id);
        payload_build_data_ack();
#if DEBUG
        debugSerial.println(F("PRENDER"));
#endif
        break;

    }
  }
}


void payload_parse_data_failure(const byte *paydata) {
  debugSerial.println(F("FAILURES"));
  byte error_qty = paydata[0];
  debugSerial.println(error_qty, DEC);
  for (i = 1; i <= 2 * error_qty; i += 2) {
    debugSerial.print(paydata[i], DEC);
    debugSerial.print(F("-->"));
    debugSerial.print(paydata[i + 1], DEC);
    debugSerial.print(F(" | "));
    //Envia errores a la pi. El primer byte es el ID y el segundo el error
    //pi_send_failure(paydata[i], paydata[i + 1]);
  }
  debugSerial.println(F(""));
}

void payload_parse_data_configuration(const byte *paydata) {
  debugSerial.println(F("CONFIGURACIONES"));
  byte io_qty = paydata[0];
  debugSerial.println(io_qty, DEC);
  for (i = 1; i <= 2 * io_qty; i += 2) {
    debugSerial.print(paydata[i], DEC);
    debugSerial.print(F("-->"));
    debugSerial.print(paydata[i + 1], DEC);
    debugSerial.print(F(" | "));
    //Envia errores a la pi. El primer byte es el ID y el segundo el tipo de sensor/actuador
    //pi_send_configuration(paydata[i], paydata[i + 1]);
  }
  debugSerial.println(F(""));
}


//Solo en cerebro
void payload_build_data_directive(const byte directiveLen, const byte *directive) {
  timestamp.value = millis() - offsetTime;
  payload_build( TYPE_DIRECTIVE, local_id, directiveLen, directive);
}

//Nodo y Cerebro
void payload_build_data_ack() {
  //Aca hay que ver a quien se hace ack y cambiar el valor de timestamp
  payload_build( TYPE_ACK, local_id, 0, NULL);
}

//Solo en cerebro
void payload_build_data_clockReset() {
  payload_build( TYPE_CLOCK_RESET, 0, 0, NULL);
}


//A HACER!!!!
//Solo en cerebro
void payload_build_data_pollingCapacity() {
  payload_build( TYPE_POLLING_CAPACITY, local_id, 0, NULL);
}

//A HACER!!!!
void payload_build_data_pollingMeasurements() {
  payload_build( TYPE_POLLING_DATA, local_id, 0, NULL);
}

//Solo en cerebro
void payload_build_data_pollingFailures() {
  payload_build( TYPE_POLLING_FAILURES, local_id, 0, NULL);
}

//Antes de llamar a esta funcion, timestamp tiene que tener el valor correspondiente.
void payload_build( const byte type, const byte local_id, const byte dataLen, const byte *data) {
#if DEBUG
  debugSerial.println(F("ENTRAMOS A PAYLOAD BUILD"));
#endif
  frame.paydata[0] = type;

  frame.paydata[1] = timestamp.valueArray[0];
  frame.paydata[2] = timestamp.valueArray[1];

  frame.paydata[3] = local_id;

  for ( i = 4; i < (4 + dataLen); i++) {
    frame.paydata[i] = data[i - 4];
  }

  frame.paylen = 4 + dataLen;

}



void sendResponseMeasurements(const int16_t* medidas, const byte cantMedidas) {
  String body = "[";
  for ( byte i = 0; i < (cantMedidas - 1); i++ ) {
    body += String(medidas[i]) + ",";
  }
  body += String(medidas[cantMedidas - 1]);
  body += "]";
  debugSerial.println(body);
  
}

void sendResponseOK(){
  debugSerial.println(F("OK"));
}

void sendResponseERROR(){
  debugSerial.println(F("ERROR"));
}

