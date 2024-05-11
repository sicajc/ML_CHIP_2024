// BEGIN stim.cpp (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See stim.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "stim.h"

void stim::stim_thread(void)
{
  wait(2, SC_NS); // Get off zero
  for (unsigned i = 0; i != 5; i++)
  {
    cout << "INFO: " << name() << " Sending event at " << sc_time_stamp() << endl;
    irq_op->notify(SC_ZERO_TIME);
    wait(5, SC_NS);
  } // endfor
} // end stim::stim_thread

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: stim.cpp,v 1.1 2004/02/10 22:47:00 dcblack Exp $
