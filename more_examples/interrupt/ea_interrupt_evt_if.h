#ifndef EA_INTERRUPT_EVT_IF_H
#define EA_INTERRUPT_EVT_IF_H
// BEGIN ea_interrupt_evt_if.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  DESCRIPTION
//    This is a custom interface that supports the notion of
//    sending events between modules. To send an event, simply invoke
//    notify with the appropriate arguments. The receiving modules should
//    wait on the default event.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct ea_interrupt_evt_if : public sc_interface
{
  virtual const sc_event &default_event() const = 0;
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: ea_interrupt_evt_if.h,v 1.2 2004/04/15 17:41:09 dcblack Exp $
