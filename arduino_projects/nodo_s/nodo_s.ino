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

extern char state;
extern char event;
extern unsigned long timeout_prevM;
extern char sending_attempts;
extern bool recon_state;
#if DEBUG
unsigned long prevMillisDebugFrame;
#endif

void setup() {

  Serial.begin( PLC_BAUDRATE );

  pinMode(A0, INPUT);
  randomSeed(analogRead(A0));

  current_init();

  mac_prevM = millis();

  //analogReference(EXTERNAL);
  //analogRead(A0);

#if DEBUG
  debugSerial.begin( 57600 );
  debugSerial.println( F("START") );
  prevMillisDebugFrame = millis();


  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);


#endif
  state_init();
}



void loop() {

  processFrame();

  state_action();
}


inline void processFrame() {
  checkMACTimeout();
  if ( mac_processFrame ) {
#if DEBUG
    debugSerial.println(F("Frame leido"));
#endif
    if ( frame_parse() ) {
#if DEBUG
      debugSerial.println("Entra a frameparse");
#endif
      event = EVENT_NOTHING;
      //Aca se procesa y ejecuta todo lo necesario.
      byte type = payload_parse( frame.paylen, frame.paydata );
#if DEBUG
      debugSerial.print("Tipo: ");
      debugSerial.println(type, HEX);
#endif
      if ( recon_state == RECON_YES ) {
        switch ( type ) {
                  case 0xFF:
                    //Hubo error - guardar falla
                    event = EVENT_NOTHING;
                    break;
          case TYPE_POLLING_DATA:
            event = EVENT_MEASUREMENTS;
            break;
          case TYPE_DIRECTIVE:
            event = EVENT_DIRECTIVE;
            break;
          case TYPE_ACK:
            event = EVENT_ACK;
            break;
          case TYPE_CLOCK_RESET:
            event = EVENT_CLKRST;
            break;
          case TYPE_POLLING_CAPACITY:
            event = EVENT_CAPACITY;
            break;
          case TYPE_POLLING_FAILURES:
            event = EVENT_FAILURES;
            break;
          default:
            event = EVENT_NOTHING;
            break;
        }
      } else if  ( ( type == TYPE_RECON ) && ( recon_state == RECON_NO ) ) {
        //no estoy reconocido y pidieron broadcast de reconocimiento
        event = EVENT_RECON;
      } 
    }
    mac_processFrame = false;
    mac_byteAct = 0;
    mac_ignore = false;
  }
}


void serialEvent() {
  //No puedo consumir lo que hay en serial si estoy en backoff de reconocimiento, lo preciso para saber si otro nodo escribio o no.
  if ( state != STATE_RECON_BACKOFF ) {
    while ( Serial.available() ) {
      byte aux = Serial.read();
#if DEBUG
      debugSerial.print(aux, HEX);
      debugSerial.print('|');
#endif
      mac_prevM = millis();
      mac_processFrame = read_check_frame( aux );
    }
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

