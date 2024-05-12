#ifndef FIFO_H
#define FIFO_H

#include <USER_DEFINED_PARAM.h>
class fifo
{
public:
  bool full;
  bool empty;

  flit_size_t regs[BUFFER_SIZE - 1];
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
  void print_fifo();
  flit_size_t flit_out();
  sc_uint<3> pntr;

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

void fifo::print_fifo()
{
  cout << "FIFO: "<<endl;
  for (int i = 0; i < pntr; i++)
  {
    sc_lv<34> temp = regs[i];
    // print out the flit and its floating point format 31~0

    // if first bit of flit is 1, it is a header flit, then dont print it as float
    if(temp[33] == 1)
    {

      cout <<"Header: "<<temp << endl;
    } else
    {
      sc_lv<32> flit_data = temp.range(31, 0);
      print_sc_lv_as_float(flit_data);
    }

  }
  cout << endl;
}

#endif // FIFO_H
