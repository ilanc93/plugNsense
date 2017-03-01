#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <Arduino.h>


#define TYPE_POLLING_DATA             0
#define TYPE_SEND_DATA                1
#define TYPE_DIRECTIVE                2
#define TYPE_ACK                      3
#define TYPE_FAILURE                  4
#define TYPE_CLOCK_RESET              5
#define TYPE_POLLING_CAPACITY         6
#define TYPE_SEND_CAPACITY            7
#define TYPE_POLLING_FAILURES         8


#define DIRECTIVE_TURN_OFF            0
#define DIRECTIVE_TURN_ON             1


byte payload_parse(const byte paylen, const byte *paydata);

void payload_parse_data_measurements(const byte *paydata);

void payload_parse_data_directive(const byte paylen, const byte *paydata, const byte local_id);

void payload_parse_data_failure(const byte *paydata);

void payload_parse_data_configuration(const byte *paydata);

//Solo en cerebro
void payload_build_data_directive(const byte directiveLen, const byte *directive);

//Nodo y Cerebro
void payload_build_data_ack();

//Solo en cerebro
void payload_build_data_clockReset();

//Solo en cerebro
void payload_build_data_pollingCapacity();

//Solo en cerebro
void payload_build_data_pollingFailures();

void payload_build_data_pollingMeasurements();


//Antes de llamar a esta funcion, timestamp tiene que tener el valor correspondiente.
void payload_build( const byte type, const byte local_id, const byte dataLen, const byte *data);


void sendResponseMeasurements(const int16_t* medidas, const byte cantMedidas);

void sendResponseOK();

void sendResponseERROR();

#endif
