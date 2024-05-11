// BEGIN main.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See interrupt.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <iostream>
using std::cout;
using std::endl;

#include <systemc.h>
#include "interrupt.h"

unsigned errors = 0;
char *simulation_name = "interrupt";

int sc_main(int argc, char *argv[])
{
  cout << "INFO: Elaborating " << simulation_name << endl;
  // sc_set_time_resolution(1,SC_PS);
  // sc_set_default_time_unit(1,SC_NS);
  interrupt interrupt_i("interrupt_i");
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
// END $Id: main.cpp,v 1.1 2004/02/10 22:47:00 dcblack Exp $
