//BEGIN interrupt.cpp
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//See interrupt.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "interrupt.h"
#include "stim.h"
#include "resp.h"
#include "ea_interrupt.h"

// Constructor
interrupt::interrupt(sc_module_name nm) {
  irq = new ea_interrupt();
  stim_i = new stim("stim_i");
  resp_i = new resp("resp_i");
  stim_i->irq_op(*irq);
  resp_i->irq_ip(*irq);
}//end interrupt::interrupt

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: interrupt.cpp,v 1.1 2004/02/10 22:47:00 dcblack Exp $
