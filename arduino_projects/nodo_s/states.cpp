#include <Arduino.h>
#include "states.hpp"
#include <SoftwareSerial.h>
extern SoftwareSerial debugSerial;

extern byte local_id;
extern byte mac_self;
extern byte mac_random;
extern byte current_actualSample[ current_sensors ];
extern myFrame frame;

extern char state;
extern char event;

extern unsigned long timeout_prevM;

extern char sending_attempts;

extern uint32_t offsetTime;

extern bool recon_state;

void state_init() {
  state = STATE_WAITING;
  event = EVENT_NOTHING;
  recon_state = RECON_NO;
  mac_self = MAC_BROADCAST;
  mac_random = generate_random_mac();
  
}

void state_action() {
  switch ( state ) {
    case STATE_WAITING:
      state_waiting_action();
      break;
    case STATE_SENDING_DATA:
      state_sending_data_action();
      break;
    case STATE_WAITING_ACK:
      state_waiting_ack_action();
      break;
    case STATE_SENDING_FAILURES:
      state_sending_failures_action();
      break;
    case STATE_CAPACITY:
      state_capacity_action();
      break;
    case STATE_WAITING_RECON:
      state_waiting_recon();
      break;
    case STATE_RECON_WATING_MAC:
      state_waiting_recon_mac();
      break;
    case STATE_WAITING_RECON_ACK:
      state_waiting_recon_ack();
      break;
  }
}

void state_waiting_action() {
  checkAndTakeCurrentMeasurements();
  //State events
  switch ( event ) {
    case EVENT_CLKRST:
      clkRst();
      break;
    case EVENT_DIRECTIVE:
      payload_parse_data_directive(frame.paylen - 4, frame.paydata + 4);
      state = STATE_WAITING_ACK;
      timeout_prevM = millis();
      sending_attempts = 1;
      //Send ACK
      sendACK();
      break;
    case EVENT_MEASUREMENTS:
      state = STATE_SENDING_DATA;
      timeout_prevM = millis();
      sending_attempts = 1;
      //Send DATA.
      sendData();
      break;
    case EVENT_FAILURES:
      state = STATE_SENDING_FAILURES;
      timeout_prevM = millis();
      sending_attempts = 1;
      //Send FAILURES.
      //A HACER LA FUNCION
      sendFailures();
      break;
  }
  event = EVENT_NOTHING;
}

void state_waiting_recon() {
  if ( event == EVENT_RECON ) {
    state = STATE_RECON_BACKOFF;
    reconBackoff();
  }
}

void state_sending_data_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_ACK ) {
    if ( sending_attempts < MAX_SENDING_ATTEMPTS ) {
      timeout_prevM = millis();
      //Send DATA.
      sendData();
      sending_attempts++;
    } else {
      state = STATE_WAITING;
      event = EVENT_NOTHING;
      //Clear data
      current_actualSample[local_id] = 0;
      //Save failure
    }
  }

  if ( event == EVENT_ACK ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
    //Clear data
    current_actualSample[local_id] = 0;
  }
}


void state_waiting_ack_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_ACK ) {
    if ( sending_attempts < MAX_SENDING_ATTEMPTS ) {
      timeout_prevM = millis();
      //Send ACK.
      sendACK();
      sending_attempts++;
    } else {
      state = STATE_WAITING;
      event = EVENT_NOTHING;
      //Save failure
    }
  }

  if ( event == EVENT_ACK ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
  }

}

void state_waiting_recon_mac() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_ACK ) {
    timeout_prevM = millis();
    state = STATE_WAITING_RECON;
  }
  if ( event == EVENT_RECON ) {
    bool ok = parse_mac_id(frame.paylen - 4, frame.paydata + 4);
    if ( ok ) {
      state = STATE_WAITING_RECON_ACK;
      timeout_prevM = millis();
      recon_state = RECON_YES;
      sendACK();
    } else {
      state = STATE_WAITING_RECON;
      event = EVENT_NOTHING;
    }
  }
}

void state_waiting_recon_ack() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_ACK ) {
    state = STATE_WAITING_RECON;
    event = EVENT_NOTHING;
    //BORRO MI DIRECCION Y MI ESTADO DE RECON
    recon_state = RECON_NO;
    mac_self = MAC_BROADCAST;
    mac_random = generate_random_mac();
  } else {
    //Nodo configurado
    //GUARDO DIRECCION EN MEMORIA NO VOLATIL

    //VAMOS A ESTE ESTADO?????
    state = STATE_CAPACITY;
  }
}

void state_sending_failures_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_ACK ) {
    if ( sending_attempts < MAX_SENDING_ATTEMPTS ) {
      timeout_prevM = millis();
      //Send Failures.
      sendFailures();
      sending_attempts++;
    } else {
      state = STATE_WAITING;
      event = EVENT_NOTHING;
      //Save failure
    }
  }

  if ( event == EVENT_ACK ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
  }
}

void clkRst() {
#if DEBUG
  debugSerial.println("HICE CLKRST EN NODO");
#endif
  offsetTime = millis();
}

void state_capacity_action() {
  //A HACER
}


void sendACK() {
  payload_build_data_ack();
  frame_build();
  frame_send();
}

void sendData() {
  payload_build_data_measurments(local_id);
  frame_build();
  frame_send();
}

void sendFailures() {
  payload_build_data_failures(0, NULL);
  frame_build();
  frame_send();
}

//ESTA FUNCION VA ACA O EN OTRA "CLASE"????? PORQUE ES CAPA APLICACION PERO TAMBIEN TIENE UN frame_send
//No es capa aplicacion, en todo caso seria capa enlace
void reconBackoff() {
  //Creo que esto esta mal, deberiamos implementar slots de tiempos en los cuales entre todo un SOF y con sobra, cosa que mientras voy haciendo backoff, miro si escribio alguien.
  //Habria que hacer un flujo un poco mas complicado, esta bien la idea de hacer random(0, BACKOFF) pero eso va a una variable auxiliar que es un limite de un for o algo asi.
  unsigned long prevM = millis();
  unsigned long backoff_time = (unsigned long) random(0, BACKOFF_TIME);
  payload_build_recon();
  frame_build();
  while ( (millis() - prevM) < backoff_time );
  
  //Preparo la trama de reconocimiento para no consumir procesamiento entre backoff y el envio.
  boolean someone_has_written = false;
  if ( Serial.available() ){
    someone_has_written = true;
    while ( Serial.available() ) {
      Serial.read();
    }
  }
  
  if ( someone_has_written ) {
    //Alguien escribio serial durante mi backoff time.
    //Podria haber escrito el 00 y sale como NULL.
    state = STATE_WAITING_RECON;
  } else {
    //Voy a enviar para reconocimiento.
    frame_send();
    state = STATE_RECON_WATING_MAC;
  }
}
