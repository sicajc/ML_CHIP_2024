#ifndef INTERRUPT_H
#define INTERRUPT_H
// BEGIN interrupt.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  DESCRIPTION
//    This design models a simple channel ea_interrupt that
//    allows modules to be connected by way of sc_event's.
//
//  DESIGN HIERARCHY
//    sc_main()
//    +- interrupt_i
//     +- stim_i # generates stimulus in the form of events
//     |    v
//     |  ea_interrupt
//     |    v
//     +- resp_i # responds to events with text output
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <systemc.h>

class ea_interrupt;
class stim;
class resp;
SC_MODULE(interrupt)
{
  ea_interrupt *irq;
  stim *stim_i;
  resp *resp_i;
  SC_HAS_PROCESS(interrupt);
  interrupt(sc_module_name nm);
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: interrupt.h,v 1.3 2004/04/15 17:41:20 dcblack Exp $
