#ifndef EA_HEARTBEAT_IF_H
#define EA_HEARTBEAT_IF_H
//BEGIN ea_interrupt_if.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DESCRIPTION
//   This is a custom hierarchical channel that supports the notion of a
//   heartbeat function similar to a clock except it is more efficient
//   than an sc_signal<bool>. It may be used anywhere in the hierachy.
//   Modules using this channel should simply wait for one of the
//   defined events.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <systemc.h>

struct ea_heartbeat_if: public sc_interface {
  virtual const sc_event& default_event() const = 0;
  virtual const sc_event& posedge_event() const = 0;
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: ea_heartbeat_if.h,v 1.2 2004/04/15 17:40:05 dcblack Exp $
