#ifndef MEM_RTL_H
#define MEM_RTL_H
//BEGIN: mem_bsyn.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//See hier_chan.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "ea_heartbeat_if.h"

SC_MODULE(mem_bsyn) {
  sc_port<ea_heartbeat_if> ck; // clock
  sc_in<bool>              ld; // load/execute command
  sc_in<bool>              rw; // read high/write low
  sc_in<unsigned long>     a;  // address
  sc_inout_rv<32>          d;  // data
  SC_HAS_PROCESS(mem_bsyn);
  explicit mem_bsyn(sc_module_name nm,
               unsigned long ba, unsigned sz) 
			   : sc_channel(nm), m_base(ba), m_size(sz), FLOAT(sc_string("0xZZZZZZZZ"))
  {
    m_mem = new long[m_size];
    SC_THREAD(mem_thread);
  }
  ~mem_bsyn() { delete [] m_mem; }
  // Processes
  void mem_thread();
  sc_signal<bool> err; // for debug
private:
  const sc_signal_rv<32> FLOAT;
  // Local data
  unsigned long m_base;
  unsigned      m_size;
  long*         m_mem;
  // Disabled methods
  mem_bsyn(const mem_bsyn&);
};
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//END $Id: mem_bsyn.h,v 1.2 2004/04/15 17:40:48 dcblack Exp $
