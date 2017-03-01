#ifndef STATES_H
#define STATES_H

#include <Arduino.h>
#include "config.hpp"
#include "current.hpp"
#include "datalink.hpp"
#include "payload.hpp"
#include <SoftwareSerial.h>

///////////   BRAIN STATES ////////
#define STATE_WAITING                0
#define STATE_WAITING_DATA           1
#define STATE_WAITING_ACK_DIRECTIVE  4
#define STATE_WAITING_FAILURES       2
#define STATE_WAITING_CAPACITY       3
#define STATE_WAITING_RECON          5
#define STATE_WAITING_ACK_RECON      6
///////////   BRAIN STATES ////////

///////////   BRAIN EVENTS ////////
#define EVENT_TIMEOUT                  20
#define EVENT_NOTHING                  0
#define EVENT_ACK                      1
#define EVENT_DATA_RECEIVED            2
#define EVENT_FAILURES_RECEIVED        3
#define EVENT_CAPACITY_RECEIVED        4
#define EVENT_RECON_RECEIVED           5
#define MASTER_EVENT_CLKRST            10
#define MASTER_EVENT_DIRECTIVE         11
#define MASTER_EVENT_MEASUREMENTS      12
#define MASTER_EVENT_FAILURES          13
#define MASTER_EVENT_CAPACITY          14
#define MASTER_EVENT_RECON             15
///////////   BRAIN EVENTS ////////



void state_init();

void state_action();

void state_waiting_action();

void state_waiting_data_action();

void state_waiting_ack_directive_action();

void state_waiting_failures_action();

void state_waiting_capacity_action();

void state_waiting_recon_action();

void state_waiting_ack_recon_action();

void send3ACK();

void sendACK();

void send3CLKRST();

void sendDirective();

void sendPollingMeasurements();


#endif
