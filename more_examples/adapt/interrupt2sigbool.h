#ifndef EA_INTERRUPT2SIGBOOL_H
#define EA_INTERRUPT2SIGBOOL_H
//BEGIN interrupt2sigbool.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// DESCRIPTION
//   This is an adaptor that converts send of events from one module
//   that previously used ea_interrupt_if into an sc_signal<bool>.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "ea_interrupt_gen_if.h"

struct interrupt2sigbool
: public sc_prim_channel
, public ea_interrupt_gen_if
, public sc_signal_in_if<bool>
{
  explicit interrupt2sigbool() 
  : sc_prim_channel(sc_gen_unique_name("interrupt2sigbool")) {}
  explicit interrupt2sigbool(sc_module_name nm) 
  : sc_prim_channel(nm) {}
  void notify() { m_delay = SC_ZERO_TIME; request_update(); }
  void notify(sc_time t) { m_delay = t; request_update(); }
  // get the value changed event
  const sc_event& value_changed_event() const { return m_interrupt; }
  const sc_event& posedge_event() const { return value_changed_event(); }
  const sc_event& negedge_event() const { return value_changed_event(); }
  const sc_event& default_event() const { return value_changed_event(); }
  // current value is true if last delta cycle was active
  const bool& read() const { m_val = event(); return m_val; }
  // get a reference to the current value (for tracing)
  const bool& get_data_ref() const { return read(); }
  // was there a value changed event?
  bool event() const { return ( simcontext()->delta_count() == m_delta + 1 ); }
  bool posedge() const { return event(); }
  bool negedge() const { return event(); }
  const sc_signal_bool_deval& delayed() const {
      const sc_signal_in_if<bool>* iface = this;
      return RCAST<const sc_signal_bool_deval&>( *iface );
  }

protected:
    // every update is a change
    virtual void update() {
      m_interrupt.notify(m_delay);
      m_delta = simcontext()->delta_count();
    }

private:
  sc_event m_interrupt;
  mutable bool m_val;  
  sc_time  m_delay;  
  uint64   m_delta;    // delta of last event
  // Copy constructor so compiler won't create one
  interrupt2sigbool( const interrupt2sigbool& );
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: interrupt2sigbool.h,v 1.3 2004/04/15 17:36:42 dcblack Exp $
