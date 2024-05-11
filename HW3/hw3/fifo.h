#include <USER_DEFINED_PARAM.h>
struct fifo
{
  // A typical fifo for storing packets
  flit_size_t regs[BUFFER_SIZE-1];
  bool full;
  bool empty;
  sc_uint<3> pntr;

  // constructor
  fifo()
  {
    full = false;
    empty = true;
    pntr = 0;
  };

  // methods
  void flit_in(const flit_size_t data_pkt);

  flit_size_t flit_out();
};
