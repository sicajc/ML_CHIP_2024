#undef SC_INCLUDE_FX

#include "USER_DEFINED_PARAM.h"
#include "fifo.h"

void fifo::flit_in(flit_size_t data_pkt)
{
  regs[pntr++] = data_pkt;
  empty = false;
  if (pntr == 4)
    full = true;
}

flit_size_t fifo::flit_out()
{
  flit_size_t temp;
  temp = regs[0];
  if (--pntr == 0)
    empty = true;
  else
  {
    regs[0] = regs[1];
    regs[1] = regs[2];
    regs[2] = regs[3];
    regs[3] = regs[4];
  }
  return (temp);
}
