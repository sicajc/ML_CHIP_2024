#ifndef USER_DEFINED_PARAM_H
#define USER_DEFINED_PARAM_H

#include "systemc.h"
#define BUFFER_SIZE 5
typedef sc_lv<34> flit_size_t;

// Encoding for directions
// 0: North, 1: East, 2: South, 3: West, 4: Core
static const int North = 0;
static const int East = 1;
static const int South = 2;
static const int West = 3;
static const int Core = 4;

// fifo control states (G)
static const int G_IDLE = 0;
static const int G_ROUTING = 1;
static const int G_WAITING_OUTPUT = 2;
static const int G_ACTIVE = 3;
static const int G_WAITING_ACK = 4;

// output buffer state
static const int O_IDLE = 0;
static const int O_WAITING_ACK = 1;
static const int O_ACTIVE = 2;
static const int O_DONE = 3;


#endif
