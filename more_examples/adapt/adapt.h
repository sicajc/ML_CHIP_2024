#ifndef ADAPT_H
#define ADAPT_H
// BEGIN adapt.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  DESCRIPTION
//    This design models a simple channel adaptor for ea_interrupt that
//    allows modules to be connected by way of sc_event's connected to
//    sc_signal<bool>.
//
//  DESIGN HIERARCHY
//    sc_main()
//    +- adapt_i
//     +- stim_i # generates stimulus in the form of events
//     |    v
//     |  interrupt2sigbool # converts notifications to bool events
//     |    v
//     +- resp_i # responds to events with text output
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <systemc.h>

// class ea_interrupt;
class interrupt2sigbool;
class stim;
class resp_rtl;
SC_MODULE(adapt)
{
  // ea_interrupt* irq;
  interrupt2sigbool *irq;
  stim *stim_i;
  resp_rtl *resp_i;
  SC_HAS_PROCESS(adapt);
  adapt(sc_module_name nm);
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: adapt.h,v 1.2 2004/04/15 17:36:23 dcblack Exp $
