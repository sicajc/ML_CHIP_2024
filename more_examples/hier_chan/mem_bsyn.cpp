// BEGIN: mem_bsyn.cpp (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See hier_chan.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SC_USE_SC_STRING_OLD
#include <systemc.h>
#include "mem_bsyn.h"

void mem_bsyn::mem_thread()
{
  bool reading = false;
  bool m_rw;
  unsigned long addr;
  // Reset errors & float the data bus
  err.write(false);
  d->write(FLOAT);
  for (;;)
  {
    // Wait for a load instruction (active high) at a clock edge
    while (ld->read() == false)
    {
      wait(ld->value_changed_event()); // wait here improves efficiency
      wait(ck->posedge_event());
    };
    addr = a->read();
    if (rw->read())
    { // reading
      // Allow for back-to-back reads. Verify legal addresses
      while (ld->read() && m_base <= addr && addr < m_base + m_size)
      {
        wait(ck->posedge_event());
        d->write(m_mem[addr - m_base]);
        addr = a->read();
        if (ld->read() && !rw->read())
        { // illegal write
          cout << "ERROR:" << name() << "@" << sc_time_stamp()
               << ": Illegal write ignored immediately following read: " << addr << endl;
          err.write(true);
        } // endif
      } // endwhile
      // remove data and errors at next clock edge
      wait(ck->posedge_event());
      d->write(FLOAT);
      err.write(false);
    }
    else
    {
      // writing
      // Validate address and write
      if (m_base <= addr && addr < m_base + m_size)
      {
        m_mem[addr - m_base] = d->read().to_long();
      } // endif
      wait(ck->posedge_event());
    } // endif
  } // endforever
} // end mem_bsyn::mem_thread

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: mem_bsyn.cpp,v 1.1 2004/03/03 03:18:36 dcblack Exp $
