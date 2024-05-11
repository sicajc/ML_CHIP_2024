#ifndef MEM_ARCH_H
#define MEM_ARCH_H
// BEGIN: mem_arch.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See hier_chan.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "CPU_if.h"

extern unsigned errors;

struct mem_arch : public sc_channel, CPU_if
{
  // Constructors & destructor
  explicit mem_arch(sc_module_name nm,
                    unsigned long ba, unsigned sz)
      : sc_channel(nm), m_base(ba), m_size(sz)
  {
    m_mem = new long[m_size];
  }
  ~mem_arch() { delete[] m_mem; }
  // Interface implementations
  void write(unsigned long addr, long data)
  {
    if (m_base <= addr && addr < m_base + m_size)
    {
      m_mem[addr - m_base] = data;
    }
  }
  long read(unsigned long addr)
  {
    if (m_base <= addr && addr < m_base + m_size)
    {
      return m_mem[addr - m_base];
    }
    else
    {
      cout << "ERROR:" << name() << "@" << sc_time_stamp()
           << ": Illegal address: " << addr << endl;
      return 0L;
    }
  }

private:
  unsigned long m_base;
  unsigned m_size;
  long *m_mem;
  mem_arch(const mem_arch &); // Disable
};
#endif

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: mem_arch.h,v 1.3 2004/05/05 19:46:22 dcblack Exp $
