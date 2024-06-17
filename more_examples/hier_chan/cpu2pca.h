#ifndef CPU2PCA_H
#define CPU2PCA_H
// BEGIN: cpu2pca.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See hier_chan.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "CPU_if.h"
#include "ea_heartbeat_if.h"

SC_MODULE(cpu2pca), CPU_if
{
  // Ports
  sc_port<ea_heartbeat_if> ck; // clock
  sc_out<bool> ld;             // load/execute command
  sc_out<bool> rw;             // read high/write low
  sc_out<unsigned long> a;     // address
  sc_inout_rv<32> d;           // data
  // Constructor
  (cpu2pca) : FLOAT(sc_string("0xZZZZZZZZ"))
  {
    // SC_THREAD(cpu2pca_thread);
    // sensitive << ck;
  }
  // Processes
  // void cpu2pca_thread(); // used for non-blocking implementations
  // Interface implementations
  void write(unsigned long addr, long data);
  long read(unsigned long addr);
  void burst_write(unsigned long addr, unsigned size, long *data);
  void burst_read(unsigned long addr, unsigned size, long *data);
  bool nb_write(unsigned long addr, long &data);
  bool nb_read(unsigned long addr, long &data);

private:
  const sc_signal_rv<32> FLOAT;
  cpu2pca(const cpu2pca &); // Disable
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: cpu2pca.h,v 1.2 2004/04/15 17:40:22 dcblack Exp $
