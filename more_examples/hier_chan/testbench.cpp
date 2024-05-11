// BEGIN: testbench.cpp (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See hier_chan.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SC_USE_SC_STRING_OLD
#include <systemc.h>
#include "testbench.h"

void testbench::test_thread()
{
  unsigned long addr;
  long data;
  srand(0);
  done->write(false);
  cout << "INFO: " << name() << " @ " << sc_time_stamp() << " Starting..." << endl;
  // Randomly read & write values;
  for (unsigned i = 0; i != m_tests; i++)
  {
    addr = m_min_addr + rand() % (m_max_addr - m_min_addr);
    if (rand() & 1)
    {
      data = rand();
      mem->write(addr, data);
      cout << "INFO: " << name() << " @ " << sc_time_stamp() << " " << i << " : "
           << "write(" << addr << ", " << data << ")"
           << endl;
    }
    else
    {
      data = mem->read(addr);
      cout << "INFO: " << name() << " @ " << sc_time_stamp() << " " << i << " : "
           << "read(" << addr << ") = " << data << ")"
           << endl;
    } // endif
  } // endfor
  cout << "INFO: " << name() << " @ " << sc_time_stamp() << " Finished!" << endl;
  done->write(true);
} // end testbench::test_thread()

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: testbench.cpp,v 1.2 2004/05/05 19:46:22 dcblack Exp $
