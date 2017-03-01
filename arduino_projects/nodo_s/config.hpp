#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>
#include "commons.hpp"


#define DEBUG 1


////////////// INICIO CORRIENTE //////////////
//Asumimos que cada sensor de corriente tiene como maximo 89 muestras.
//Estas corresponden a un tiempo de aproximadamente 6 minutos si se toma una muestra cada 4 segundos.

//Tiempo entre muestras en milisegundos.
const uint32_t current_sampling_period = 4000;

//Cantidad de sensores de corriente;
const byte current_sensors = 2;

const char current_pins[current_sensors] = {A0, A1};

const char relay_pins[current_sensors] = {2, 3};

//Cantidad maxima de muestras. CON ESTE TAMANO SE CALIBRA EL LARGO DE LA TRAMA ACORDE AL POLLING TIME
const byte current_max_samples = 89;


////////////// FIN CORRIENTE //////////////


////////////// INICIO MAC //////////////


#define MAC_SOF       0xAA  //es 0xAA o 10101010
#define MAC_BRAIN     0x00  //Direccion fisica del cerebro
#define MAC_BROADCAST 0xFF  //Direccion fisica de broadcast

#define MAC_BUFFLEN   200   //Tamanio de buffer a usar para las tramas
#define MAC_TIMEOUT   5000   //En milisegundos

#define RANDOM_MAC_MIN  1
#define RANDOM_MAC_MAX  255

////////////// FIN MAC //////////////


////////////// INICIO RECON //////////////

#define RECON_YES       true  
#define RECON_NO        false
#define BACKOFF_TIME    900
////////////// FIN RECON //////////////


////////////// INICIO PLC //////////////

#define PLC_BAUDRATE 4800

//#define PLC_RX_PIN 10
//#define PLC_TX_PIN 9

////////////// FIN PLC //////////////


#define DEBUG_SERIAL_RX_PIN 10
#define DEBUG_SERIAL_TX_PIN 9



///////////   TIMEOUTS ////////
#define TIMEOUT_ACK             1000
///////////   TIMEOUTS ////////

#define MAX_SENDING_ATTEMPTS    3


#endif
