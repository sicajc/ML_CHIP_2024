// BEGIN main.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See hier_chan.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SC_USE_SC_STRING_OLD
#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "hier_chan.h"

unsigned errors = 0;
char *simulation_name = "hier_chan";

int sc_main(int argc, char *argv[])
{
  cout << "INFO: Elaborating " << simulation_name << endl;
  // sc_set_time_resolution(1,SC_PS);
  // sc_set_default_time_unit(1,SC_NS);
  hier_chan hier_chan_i("hier_chan_i");
  cout << "INFO: Simulating " << simulation_name << endl;
  sc_start();
  cout << "INFO: Post-processing " << simulation_name << endl;
  cout << "INFO: Simulation " << simulation_name
       << " " << (errors ? "FAILED" : "PASSED")
       << " with " << errors << " errors"
       << endl;
  getchar();
  return errors ? 1 : 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: main.cpp,v 1.1 2004/03/03 03:18:36 dcblack Exp $
