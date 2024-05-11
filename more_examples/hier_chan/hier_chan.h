#ifndef HIER_CHAN_H
#define HIER_CHAN_H
// BEGIN hier_chan.h
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  DESCRIPTION
//    This design contains two examples of custom hierarchical channels.
//    First, mem_arch is a architectural model of a memory using the
//    custom CPU_if interface. Second, cpu2pca is a transactor that
//    adapts the CPU_if to a pin-cycle-accurate interface using signals.
//    mem_bsyn is a pin-cycle accurate behavioral model that may be
//    synthesizable.
//
//    To simplify understanding, we've put two sets of simulations in the
//    same model, one architectural and one with pin-level behavior. As
//    detour, the ea_heartbeat model is used for a clock.
//
//    Notice that the memories and testbench are instantiated with
//    parameters that affect their configuration.
//
//  DESIGN HIERARCHY
//    sc_main()
//    +- hier_chan_i
//       +- testbench_arch_i
//       |     v
//       +- mem_arch_i (hierarchical channel)
//       |
//       +- testbench_bsyn_i
//       |     v
//       |  cpu2pca_i (transactor)
//       |     v
//       +- mem_bsyn_i
//       +- clock
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <systemc.h>
#include "testbench.h"
#include "mem_arch.h"
#include "cpu2pca.h"
#include "mem_bsyn.h"
#include "ea_heartbeat.h"

SC_MODULE(hier_chan)
{
  // Architectural model instances
  testbench testbench_arch_i;
  // Architectural model channels
  mem_arch mem_arch_i; // hierarchical channel
  // Pin-level model instances
  testbench testbench_bsyn_i;
  mem_bsyn mem_bsyn_i;
  // Pin-level model channels
  cpu2pca cpu2pca_i; // hierarchical transactor
  sc_signal<bool> ld;
  sc_signal<bool> rw;
  sc_signal<unsigned long> a;
  sc_signal_rv<32> d;
  sc_signal<bool> done_arch;
  sc_signal<bool> done_bsyn;
  ea_heartbeat clock;
  SC_CTOR(hier_chan)
      : testbench_arch_i("testbench_arch_i", 3100, 4100, 100), mem_arch_i("mem_arch_i", 3000, 1000), testbench_bsyn_i("testbench_bsyn_i", 5100, 6100, 100), cpu2pca_i("cpu2pca_i"), mem_bsyn_i("mem_bsyn_i", 5000, 1000), clock("clock", sc_time(2, SC_NS))
  {
    SC_METHOD(done_method);
    sensitive << done_arch << done_bsyn;
    dont_initialize();
    // Architectural model connectivity
    testbench_arch_i.mem(mem_arch_i);
    testbench_arch_i.done(done_arch);
    // Pin-level model connectivity
    testbench_bsyn_i.mem(cpu2pca_i);
    testbench_bsyn_i.done(done_bsyn);
    cpu2pca_i.ld(ld);
    cpu2pca_i.rw(rw);
    cpu2pca_i.a(a);
    cpu2pca_i.d(d);
    cpu2pca_i.ck(clock);
    mem_bsyn_i.ld(ld);
    mem_bsyn_i.rw(rw);
    mem_bsyn_i.a(a);
    mem_bsyn_i.d(d);
    mem_bsyn_i.ck(clock);
  }
  // This method waits for both tests to complete and then terminates SystemC
  void done_method()
  {
    if (done_arch.read() && done_bsyn.read())
      sc_stop();
  }
};

#endif
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: hier_chan.h,v 1.3 2004/05/05 19:46:21 dcblack Exp $
