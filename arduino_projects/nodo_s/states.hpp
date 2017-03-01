#ifndef STATES_H
#define STATES_H

#include <Arduino.h>
#include "config.hpp"
#include "current.hpp"
#include "datalink.hpp"
#include "payload.hpp"

///////////   NODE STATES ////////
#define STATE_WAITING           0
#define STATE_SENDING_DATA      1
#define STATE_WAITING_ACK       2
#define STATE_SENDING_FAILURES  3
#define STATE_CAPACITY          4
#define STATE_WAITING_RECON     5
#define STATE_RECON_BACKOFF     6
#define STATE_RECON_WATING_MAC  7
#define STATE_WAITING_RECON_ACK 8

///////////   NODE STATES ////////

///////////   NODE EVENTS ////////
#define EVENT_CLKRST            5
#define EVENT_ACK               3
#define EVENT_DIRECTIVE         2
#define EVENT_MEASUREMENTS      1
#define EVENT_FAILURES          8
#define EVENT_CAPACITY          9
#define EVENT_RECON             10
#define EVENT_TIMEOUT           20
#define EVENT_NOTHING           0
///////////   NODE EVENTS ////////

extern char state;
extern char event;

extern unsigned long timeout_prevM;

extern char sending_attempts;

extern uint32_t offsetTime;


void state_init();

void state_action();

void state_waiting_action();

void state_sending_data_action();

void state_waiting_ack_action();

void state_waiting_recon();
void state_waiting_recon_mac();

void state_sending_failures_action();

void state_waiting_recon_ack();

void clkRst();

void state_capacity_action();

void sendACK();

void sendData();

void sendFailures();

void reconBackoff();
#endif
