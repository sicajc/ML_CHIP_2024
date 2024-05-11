#ifndef EA_INTERRUPT_H
#define EA_INTERRUPT_H
//BEGIN ea_interrupt.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DESCRIPTION
//   This is a custom primitive channel that supports the notion of
//   sending events between modules. To send an event, simply invoke
//   notify with the appropriate arguments. The receiving modules should
//   wait on the default event.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "ea_interrupt_evt_if.h"
#include "ea_interrupt_gen_if.h"

struct ea_interrupt
: public sc_prim_channel
, public ea_interrupt_evt_if
, public ea_interrupt_gen_if
{
  explicit ea_interrupt() 
  : sc_prim_channel(sc_gen_unique_name("ea_interrupt")) {}
  explicit ea_interrupt(sc_module_name nm) 
  : sc_prim_channel(nm) {}
  void notify() { m_interrupt.notify(); }
  void notify(sc_time t) { m_interrupt.notify(t); }
  const sc_event& default_event() const { return m_interrupt; }
private:
  sc_event m_interrupt;
  // Copy constructor so compiler won't create one
  ea_interrupt( const ea_interrupt& );
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: ea_interrupt.h,v 1.2 2004/04/15 17:36:28 dcblack Exp $
