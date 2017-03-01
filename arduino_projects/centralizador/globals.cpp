#include "commons.hpp"
#include "config.hpp"
#include <SoftwareSerial.h>


//Es el tiempo absoluto del ultimo reset de clock.
uint32_t offsetTime;

//Es una variable que tiene el timestamp a mandar en donde corresponda.
myInt16_t timestamp;


//Variables auxiliares para todos los fors.
byte i, j;

////////////// INICIO FRAME/PAYLOAD //////////////
//Es la trama de datos nuestra.
myFrame frame;

byte local_id; //Es el local_id para procesar la trama.

byte directive;

byte link_buffer[MAC_BUFFLEN];

byte mac_self = 0x00; //A sacar, se autoasigna en reconocimiento o de EEPROM.

byte mac_node = 1; //A cargar con las directivas del MASTER BRAIN
////////////// FIN FRAME/PAYLOAD //////////////



////////////// INICIO CORRIENTE //////////////
//Se asume que los local_ids entre 0 y 9 inclusive son de corriente.
//Predefinimos que los local_ids correspondientes a otros tipos de "sensores/actuadores" van de 10 en 10.
myInt16_t current_samples[ current_sensors ][ current_max_samples ];

//Indica la posicion de la muestra actual.
byte current_actualSample[ current_sensors ] = {0};


//Es el tiempo absoluto de la primer muestra de samples
//Hay que ver que pasa para mandarlo cuando hagamos firstSampleTime - offsetTime < 0?;
unsigned long current_firstSampleTime[ current_sensors ];

unsigned long current_prevM;



////////////// FIN CORRIENTE //////////////

////////////// INICIO PLC //////////////
SoftwareSerial debugSerial(DEBUG_SERIAL_RX_PIN, DEBUG_SERIAL_TX_PIN);
////////////// FIN PLC //////////////


////////////// INICIO STATE MACHINE //////////////
char state;
char event;

//Variable auxiliar para todos los timeouts de la maquina de estados.
unsigned long timeout_prevM;

char sending_attempts;
////////////// FIN STATE MACHINE //////////////
