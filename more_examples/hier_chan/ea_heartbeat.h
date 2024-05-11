#ifndef EA_HEARTBEAT_H
#define EA_HEARTBEAT_H
//BEGIN ea_heartbeat.h (systemc)
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
#include "ea_heartbeat_if.h"


struct ea_heartbeat
: public sc_channel
, public ea_heartbeat_if
{
  SC_HAS_PROCESS(ea_heartbeat);
  // Constructors
  explicit ea_heartbeat() 
  : sc_channel(sc_gen_unique_name("ea_heartbeat"))
  , m_period(sc_get_default_time_unit()) {
    cout << "INFO: " << name() << " defaulting to period of " << m_period << endl;
    SC_METHOD(heartbeat_method);
    sensitive << m_heartbeat;
  }
  explicit ea_heartbeat(sc_module_name nm) 
  : sc_channel(nm)
  , m_period(sc_get_default_time_unit()) {
    cout << "INFO: " << name() << " defaulting to period of " << m_period << endl;
    SC_METHOD(heartbeat_method);
    sensitive << m_heartbeat;
  }
  explicit ea_heartbeat(sc_time _period) 
  : sc_channel(sc_gen_unique_name("ea_heartbeat"))
  , m_period(_period) {
    cout << "INFO: " << name() << " using period of " << m_period << endl;
    SC_METHOD(heartbeat_method);
    sensitive << m_heartbeat;
  }
  explicit ea_heartbeat(sc_module_name nm, sc_time _period) 
  : sc_channel(nm)
  , m_period(_period) {
    cout << "INFO: " << name() << " using period of " << m_period << endl;
    SC_METHOD(heartbeat_method);
    sensitive << m_heartbeat;
  }
  // User methods
  const sc_event& default_event() const { return m_heartbeat; }
  const sc_event& posedge_event() const { return m_heartbeat; }
  // Processes
  void heartbeat_method();
private:
  sc_event m_heartbeat;
  sc_time  m_period;
  // Copy constructor so compiler won't create one
  ea_heartbeat( const ea_heartbeat& );
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: ea_heartbeat.h,v 1.2 2004/04/15 17:40:28 dcblack Exp $
