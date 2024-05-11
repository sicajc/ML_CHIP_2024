#ifndef Interchange_H
#define Interchange_H
#include <systemc.h>

SC_MODULE(Interchange) {
    //port
    sc_in_clk        iclk;
    sc_out<bool>     LD; // load/execute command
    sc_out<bool>     RW; // read high/write low
    sc_out<unsigned> X;  // X address
    sc_out<unsigned> Y;  // Y address
    sc_inout_rv<32>  D;  // data
    //constructors
    SC_CTOR(Interchange)
    {
        reg = new int*[8];
        for(int i=0;i<8;i++)
            reg[i] = new int[8];
        SC_THREAD(interchange_thread);
        sensitive << iclk.pos();
        dont_initialize();
    }
    void interchange_thread(void);
    void rotation(void);

private:
    int** reg;
};

#endif
