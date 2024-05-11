// BEGIN: cpu2pca.h (systemc)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// See hier_chan.h for more information
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define SC_USE_SC_STRING_OLD
#include <systemc.h>
#include "cpu2pca.h"

enum operation
{
  WRITE = false,
  READ = true
};

void cpu2pca::write(unsigned long addr, long data)
{
  // cout << "INFO: " << name() << "::write starting @ " << sc_time_stamp() << endl;
  wait(ck->posedge_event());
  ld->write(true);
  rw->write(WRITE);
  a->write(addr);
  d->write(data);
  wait(ck->posedge_event());
  ld->write(false);
}

long cpu2pca::read(unsigned long addr)
{
  // cout << "INFO: " << name() << "::read starting @ " << sc_time_stamp() << endl;
  wait(ck->posedge_event());
  ld->write(true);
  rw->write(READ);
  a->write(addr);
  d->write(FLOAT);
  wait(ck->posedge_event());
  ld->write(false);
  return d->read().to_long();
}

void cpu2pca::burst_write(unsigned long addr, unsigned size, long *data)
{
  for (unsigned i = 0; i != size; i++)
  {
    wait(ck->posedge_event());
    ld->write(true);
    rw->write(WRITE);
    a->write(addr++);
    d->write(data[i]);
  } // endfor i
  wait(ck->posedge_event());
  ld->write(false);
}

void cpu2pca::burst_read(unsigned long addr, unsigned size, long *data)
{
  wait(ck->posedge_event());
  ld->write(true);
  rw->write(READ);
  a->write(addr++);
  d->write(FLOAT);
  for (unsigned i = 0; i != size; i++)
  {
    wait(ck->posedge_event());
    ld->write(i != (size - 1));
    a->write(addr++);
    data[i] = d->read().to_long();
  } // endfor i
  wait(ck->posedge_event());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// END $Id: cpu2pca.cpp,v 1.1 2004/03/03 03:18:36 dcblack Exp $
