#ifndef STIM_H
#define STIM_H
//BEGIN stim.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See interrupt.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <systemc.h>
#include "ea_interrupt_gen_if.h"

SC_MODULE(stim) {
  sc_port<ea_interrupt_gen_if> irq_op;
  SC_CTOR(stim)
  {
    SC_THREAD(stim_thread);
  }
  void stim_thread(void);
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: stim.h,v 1.2 2004/04/15 17:36:58 dcblack Exp $
