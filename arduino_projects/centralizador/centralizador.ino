#include "config.hpp"
#include "commons.hpp"
#include "payload.hpp"
#include "current.hpp"
#include "datalink.hpp"
#include "states.hpp"
#include <SoftwareSerial.h>

extern unsigned long current_prevM;

volatile unsigned long mac_prevM;
byte mac_byteAct = 0;
volatile boolean mac_processFrame = false;
extern byte link_buffer[MAC_BUFFLEN];
volatile boolean mac_ignore = false;
extern byte mac_self;

extern myFrame frame;

extern byte link_buffer[MAC_BUFFLEN];

extern uint32_t offsetTime;

extern SoftwareSerial debugSerial;

extern char event;
extern byte directive;

extern byte local_id;
extern byte mac_node;
#if DEBUG
unsigned long prevMillisDebugFrame;
#endif


const byte numChars = 6;
char receivedChars[numChars]; // an array to store the received data

boolean newData = false;

const char EOL = '\r';

void setup() {

  Serial.begin( PLC_BAUDRATE );


  pinMode(A0, INPUT);
  randomSeed(analogRead(A0));

  current_prevM = millis();
  mac_prevM = millis();

  //analogReference(EXTERNAL);
  //analogRead(A0);
  debugSerial.begin( 57600 );
#if DEBUG
  debugSerial.println( F("START") );
  prevMillisDebugFrame = millis();

  //Instrucciones
  debugSerial.println(F("1-Pedir Datos"));
  debugSerial.println(F("2-Enviar Directiva"));
  debugSerial.println(F("3-CLKRST"));

#endif

  state_init();

}


void loop() {

  processFrame();

  master_event();

  state_action();

  recvWithEndMarker();
  showNewData();

}


inline void processFrame() {
  checkMACTimeout();
  if ( mac_processFrame ) {
#if DEBUG
    debugSerial.println(F("Frame leido"));
#endif
    if ( frame_parse() ) {
      event = EVENT_NOTHING;
      //Aca se procesa y ejecuta todo lo necesario.

      byte type = payload_parse( frame.paylen, frame.paydata );
      switch ( type ) {
        case 0xFF:
          //Hubo error - paylen < 4
          break;
        case TYPE_SEND_DATA:
          event = EVENT_DATA_RECEIVED;
          break;
        case TYPE_ACK:
          event = EVENT_ACK;
          break;
        case TYPE_FAILURE:
          event = EVENT_FAILURES_RECEIVED;
          break;
        case TYPE_SEND_CAPACITY:
          event = EVENT_CAPACITY_RECEIVED;
          break;
        default:
          break;
      }
    }
    mac_processFrame = false;
    mac_byteAct = 0;
    mac_ignore = false;
  }
}



void serialEvent() {
  while ( Serial.available() ) {
    byte aux = Serial.read();
    //debugSerial.print(aux, HEX);
    //debugSerial.print('|');
    mac_prevM = millis();
    mac_processFrame = read_check_frame( aux );
  }
}


inline boolean read_check_frame( const byte aux ) {
  switch ( mac_byteAct ) {
    case 0:
    case 1:
    case 2:
    case 3:
      if ( aux != MAC_SOF ) {
        mac_byteAct = 0;
        return false;
      }
      break;
    case 4:
      //Hay que ver que pasa cuando aun no se esta reconocido (mac_self es cualquier cosa).
      if ( aux != mac_self && aux != MAC_BROADCAST ) {
        mac_ignore = true;
      }
      break;
    /*
      case 2:
      break;
      case 3:
      break;
    */
    //No hace nada
    default:
      break;
  }

  link_buffer[mac_byteAct] = aux;
  mac_byteAct++;

  mac_prevM = millis();

  //Estamos en paydata y justo llego el checksum.
  if ( mac_byteAct > 7 && ( mac_byteAct == (link_buffer[6] + 8) ) ) {
    if ( mac_ignore ) {
      mac_byteAct = 0;
      mac_ignore = false;
      return false;
    }
    return true;
  }
  return false;
}


void checkMACTimeout() {
  if ( (millis() - mac_prevM) >= MAC_TIMEOUT ) {
    mac_byteAct = 0;
    mac_processFrame = false;
    mac_ignore = false;
  }
}


void master_event() {
  if (debugSerial.available()) {
    char aux = debugSerial.read();
    frame.orig = 0;
    switch (aux) {
      case '1':
        debugSerial.print(F("1-Elegir nodo"));
        while (!debugSerial.available());
        mac_node = debugSerial.read() - '0';
        debugSerial.println(mac_node, DEC);
        frame.dest = mac_node;
        debugSerial.print(F("Ingrese local_id del sensor: "));
        while (!debugSerial.available());
        local_id = debugSerial.read() - '0';
        debugSerial.println(local_id, DEC);
        event = MASTER_EVENT_MEASUREMENTS;
        debugSerial.println("MASTER_EVENT_MEASUREMENTS");
        break;
      case '2':
        event = MASTER_EVENT_DIRECTIVE;
        debugSerial.println(F("1-Elegir nodo"));
        while (!debugSerial.available());
        mac_node = debugSerial.read() - '0';
        frame.dest = mac_node;
        debugSerial.print(F("Ingrese local_id del sensor: "));
        while (!debugSerial.available());
        local_id = debugSerial.read() - '0';
        debugSerial.println(local_id, DEC);
        debugSerial.println(F("0-Apagar / 1-Prender"));
        while (!debugSerial.available());
        directive = debugSerial.read() - '0';
        debugSerial.println(directive, DEC);
        debugSerial.println("MASTER_EVENT_DIRECTIVE");
        break;
      case '3':
        event = MASTER_EVENT_CLKRST;
        debugSerial.println("MASTER_EVENT_CLKRST");
        break;
      case '4':
        event = MASTER_EVENT_FAILURES;
        debugSerial.println("MASTER_EVENT_FAILURES");
        break;
      case '5':
        event = MASTER_EVENT_CAPACITY;
        debugSerial.println("MASTER_EVENT_CAPACITY");
        break;
      case '6':
        event = MASTER_EVENT_RECON;
        debugSerial.println("MASTER_EVENT_RECON");
        break;
    }
  }
}






void recvWithEndMarker() {
  static byte ndx = 0;
  const char endMarker = '\n';
  char rc;

  // if (Serial.available() > 0) {
  while (debugSerial.available() > 0 && newData == false) {
    rc = debugSerial.read();
    
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    mac_node = (byte)(receivedChars[0] - '0');
    local_id = (byte)(receivedChars[1] - '0');
    directive = (byte)(receivedChars[2] - '0');
    switch (directive) {
      case 0:
      case 1:
        event = MASTER_EVENT_DIRECTIVE;
        break;
      case 2:
        event = MASTER_EVENT_MEASUREMENTS;
        break;
      case 3:
        event = MASTER_EVENT_CLKRST;
        break;
    }
    newData = false;
  }
}







