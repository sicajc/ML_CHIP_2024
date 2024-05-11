#ifndef Transactor_H
#define Transactor_H
#include <systemc.h>
#include "Mem_if.h"
SC_MODULE(Transactor), Mem_if {
    // Ports
    sc_in_clk		 iclk;
    sc_out<bool>     LD;   // load/execute command
    sc_out<bool>     RW;   // read high/write low
    sc_out<unsigned> X;    // X address
    sc_out<unsigned> Y;    // Y address
    sc_inout_rv<32>  D;    // data
    // Constructor
    SC_CTOR(Transactor) {}
    // Processes
    
    // Interface implementations
    
    void word_read(unsigned x, unsigned y, int& d);
    void word_write(unsigned x, unsigned y, int d);
private:

};
#endif
