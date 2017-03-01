#include "states.hpp"

extern char state;
extern char event;

extern uint32_t offsetTime;

extern unsigned long timeout_prevM;

extern char sending_attempts;

extern byte mac_node;

extern byte local_id;

extern SoftwareSerial debugSerial;

extern byte mac_self;

extern byte directive;

extern myFrame frame;

void state_init() {
  state = STATE_WAITING;
  event = EVENT_NOTHING;
  mac_node = 1;
}

void state_action() {
#if DEBUG
  //debugSerial.print(F("Estado: "));
  //debugSerial.println(state, HEX);
#endif
  switch ( state ) {
    case STATE_WAITING:
      state_waiting_action();
      break;
    case STATE_WAITING_DATA:
      state_waiting_data_action();
      break;
    case STATE_WAITING_ACK_DIRECTIVE:
      state_waiting_ack_directive_action();
      break;
    case STATE_WAITING_FAILURES:
      state_waiting_failures_action();
      break;
    case STATE_WAITING_CAPACITY:
      state_waiting_capacity_action();
      break;
    case STATE_WAITING_RECON:
      state_waiting_recon_action();
      break;
    case STATE_WAITING_ACK_RECON:
      state_waiting_ack_recon_action();
      break;
  }
}

void state_waiting_action() {
#if DEBUG
  if ( event != EVENT_NOTHING ) {
  //  debugSerial.println(event, HEX);
  }
#endif
  switch ( event ) {
    case MASTER_EVENT_CLKRST:
      send3CLKRST();
      sendResponseOK();
      break;
    case MASTER_EVENT_DIRECTIVE:
      //Send directive to NODE.
      //EN EL MOMENTO EN QUE SE SETEA EL EVENTO, SE ACTUALIZAN LAS VARIABLES GLOBALES PARA HACR EL BUILD_DATA_DIRECTIVE
      sendDirective();
      state = STATE_WAITING_ACK_DIRECTIVE;
      timeout_prevM = millis();
      sending_attempts = 1;
      break;
    case MASTER_EVENT_MEASUREMENTS:
      //Send polling MEASUREMENTS to NODE.
      sendPollingMeasurements();
      state = STATE_WAITING_DATA;
      timeout_prevM = millis();
      sending_attempts = 1;
      break;
    case MASTER_EVENT_FAILURES:
      //Send polling FAILURE to NODE.
      //payload_build_data_pollingFailures();
      //frame_build(mac_self, mac_node);
      //frame_send();
      state = STATE_WAITING_FAILURES;
      timeout_prevM = millis();
      sending_attempts = 1;
      break;
    case MASTER_EVENT_CAPACITY:
      //Send polling CAPACITY to NODE.
      //payload_build_data_pollingCapacity();
      //frame_build(mac_self, mac_node);
      //frame_send();
      state = STATE_WAITING_CAPACITY;
      timeout_prevM = millis();
      sending_attempts = 1;
      break;
    case MASTER_EVENT_RECON:
      //Send polling RECON to ALL NODES.
      //A HACER
      state = STATE_WAITING_RECON;
      timeout_prevM = millis();
      sending_attempts = 1;
      break;
  }
  event = EVENT_NOTHING;
}

void state_waiting_data_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_DATA ) {
    if ( sending_attempts < MAX_SENDING_ATTEMPTS ) {
      timeout_prevM = millis();
      //Send DATA.
      sendPollingMeasurements();
      sending_attempts++;
    } else {
      state = STATE_WAITING;
      event = EVENT_NOTHING;
      //Report failure to brain
      sendResponseERROR();
    }
  }

  if ( event == EVENT_DATA_RECEIVED ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
    //Report data to brain
    payload_parse_data_measurements( frame.paydata + 4);
    send3ACK();
  }
}

void state_waiting_ack_directive_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_ACK ) {
    if ( sending_attempts < MAX_SENDING_ATTEMPTS ) {
      timeout_prevM = millis();
      //Send directive to node.
      //Prender
      sendDirective();
      sending_attempts++;
    } else {
      state = STATE_WAITING;
      event = EVENT_NOTHING;
      //Report failure to brain
      sendResponseERROR();
    }
  }

  if ( event == EVENT_ACK ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
    //Report success/ack to brain
    sendResponseOK();
    send3ACK();
  }
}

void state_waiting_failures_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_FAILURES ) {
    if ( sending_attempts < MAX_SENDING_ATTEMPTS ) {
      timeout_prevM = millis();
      //Send polling failures to node.
      //payload_build_data_pollingFailures();
      //frame_build(mac_self, mac_node);
      //frame_send();
      sending_attempts++;
    } else {
      state = STATE_WAITING;
      event = EVENT_NOTHING;
      //Report communication failure to brain
    }
  }

  if ( event == EVENT_FAILURES_RECEIVED ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
    //Report received node failures to brain
    send3ACK();
  }
}

void state_waiting_capacity_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_CAPACITY ) {
    if ( sending_attempts < MAX_SENDING_ATTEMPTS ) {
      timeout_prevM = millis();
      //Send polling capacity to node.
      //payload_build_data_pollingCapacity();
      //frame_build(mac_self, mac_node);
      //frame_send();
      sending_attempts++;
    } else {
      state = STATE_WAITING;
      event = EVENT_NOTHING;
      //Report communication failure to brain
    }
  }

  if ( event == EVENT_CAPACITY_RECEIVED ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
    //Report received node capacities to brain
    send3ACK();
  }
}

void state_waiting_recon_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_RECON ) {
    if ( sending_attempts < MAX_SENDING_ATTEMPTS ) {
      timeout_prevM = millis();
      //Send broadcast recon to ALL NODES.
      //A HACER
      sending_attempts++;
    } else {
      state = STATE_WAITING;
      event = EVENT_NOTHING;
      //Report no new nodes to add to brain
    }
  }

  if ( event == EVENT_RECON_RECEIVED ) {
    state = STATE_WAITING_ACK_RECON;
    event = EVENT_NOTHING;
    //send MAC ADDRESS to NODE
    timeout_prevM = millis();
  }
}

void state_waiting_ack_recon_action() {
  if ( (millis() - timeout_prevM ) >= TIMEOUT_ACK ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
    //Report recon failure to brain
    sendResponseERROR();
  }

  if ( event == EVENT_ACK ) {
    state = STATE_WAITING;
    event = EVENT_NOTHING;
    //send ack to NODE
    //Report success to brain
    sendACK();
  }
}

void send3ACK() {
  payload_build_data_ack();
  frame_build();
  frame_send();
  delay(DELAY_SEND_ACK);
  frame_send();
  delay(DELAY_SEND_ACK);
  frame_send();
}

void sendACK() {
  payload_build_data_ack();
  frame_build();
  frame_send();
}

void send3CLKRST() {
  payload_build_data_clockReset();
  frame_build();
  frame_send();
  delay(DELAY_SEND_CLKRST);
  frame_send();
  delay(DELAY_SEND_CLKRST);
  frame_send();
}

void sendDirective() {
  //byte aux = DIRECTIVE_TURN_ON;

  payload_build_data_directive(1, &directive);
  frame_build();
  frame_send();
}

void sendPollingMeasurements() {
  //local_id = random(0, current_sensors); //local id;
  payload_build_data_pollingMeasurements();
  frame_build();
  frame_send();
}


