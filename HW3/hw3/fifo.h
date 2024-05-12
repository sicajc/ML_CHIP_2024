#ifndef FIFO_H
#define FIFO_H

#include <USER_DEFINED_PARAM.h>
class fifo
{
public:
  bool full;
  bool empty;

  // constructor
  fifo()
  {
    full = false;
    empty = true;
    pntr = 0;
  };

  // A typical fifo for storing packets
  // methods
  void flit_in(const flit_size_t data_pkt);
  flit_size_t flit_out();
  sc_uint<3> pntr;

  private:
  flit_size_t regs[BUFFER_SIZE - 1];
};

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

#endif // FIFO_H
