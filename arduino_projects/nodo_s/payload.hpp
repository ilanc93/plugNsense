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
#define TYPE_SEND_CONFIGURATION       7
#define TYPE_POLLING_FAILURES         8
#define TYPE_RECON                    9


byte payload_parse(const byte paylen, const byte *paydata);

void payload_parse_data_measurements(const byte *paydata);

void payload_parse_data_directive(const byte paylen, const byte *paydata);

void payload_parse_data_failure(const byte *paydata);

void payload_parse_data_configuration(const byte *paydata);

//Solo en nodo
void payload_build_data_measurments(const byte local_id);

//Solo en cerebro
void payload_build_data_directive(const byte local_id, const byte directiveLen, const byte *directive);

//Nodo y Cerebro
void payload_build_data_ack();

//Solo en nodo
void payload_build_data_failures(const byte failureLen, const byte *failures);

//Solo en cerebro
void payload_build_data_clockReset();

//Solo en cerebro
void payload_build_data_pollingConfiguration();

//Solo en nodo
void payload_build_data_sendConfiguration(const byte ioDataLen, const byte *ioData);

//Solo en cerebro
void payload_build_data_pollingFailures();

void payload_build_recon();

bool parse_mac_id(const byte paylen, const byte *paydata);

//Antes de llamar a esta funcion, timestamp tiene que tener el valor correspondiente.
void payload_build( const byte type, const byte local_id, const byte dataLen, const byte *data);

#endif
