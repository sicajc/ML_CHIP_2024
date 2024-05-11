// BEGIN main.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See heartbeat.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "heartbeat.h"
#include "ea_heartbeat.h"

unsigned errors = 0;
char *simulation_name = "heartbeat";

int sc_main(int argc, char *argv[])
{
  cout << "INFO: Elaborating " << simulation_name << endl;
  // sc_set_time_resolution(1,SC_PS);
  sc_set_default_time_unit(100, SC_PS);
  sc_time timeout(2, SC_NS);
  ea_heartbeat clock("clock");
  heartbeat heartbeat_i("heartbeat_i");
  heartbeat_i.clock(clock);
  cout << "INFO: Simulating " << simulation_name << endl;
  sc_start(timeout);
  cout << "INFO: Post-processing " << simulation_name << endl;
  cout << "INFO: Simulation " << simulation_name
       << " " << (errors ? "FAILED" : "PASSED")
       << " with " << errors << " errors"
       << endl;
  getchar();
  return errors ? 1 : 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: main.cpp,v 1.2 2004/02/13 05:37:17 dcblack Exp $
