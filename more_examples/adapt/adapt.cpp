// BEGIN adapt.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See adapt.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "adapt.h"
#include "stim.h"
// #include "resp.h"
// #include "ea_interrupt.h"
#include "resp_rtl.h"
#include "interrupt2sigbool.h"

// Constructor
adapt::adapt(sc_module_name nm)
{
  stim_i = new stim("stim_i");
  // irq = new ea_interrupt();
  // resp_i = new resp("resp_i");
  irq = new interrupt2sigbool();
  resp_i = new resp_rtl("resp_i");
  stim_i->irq_op(*irq);
  resp_i->irq_ip(*irq);
} // end adapt::adapt

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: adapt.cpp,v 1.1 2004/02/13 05:21:42 dcblack Exp $
