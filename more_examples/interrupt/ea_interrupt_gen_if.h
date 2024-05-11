#ifndef EA_INTERRUPT_GEN_IF_H
#define EA_INTERRUPT_GEN_IF_H
//BEGIN ea_interrupt_gen_if.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DESCRIPTION
//   This is a custom primitive channel that supports the notion of
//   sending events between modules. To send an event, simply invoke
//   notify with the appropriate arguments. The receiving modules should
//   wait on the default event.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct ea_interrupt_gen_if: public sc_interface {
  virtual void notify() = 0;
  virtual void notify(sc_time t) = 0;
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: ea_interrupt_gen_if.h,v 1.2 2004/04/15 17:41:15 dcblack Exp $
