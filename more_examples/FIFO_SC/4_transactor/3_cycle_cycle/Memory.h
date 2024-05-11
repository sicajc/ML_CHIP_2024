#ifndef Memory_H
#define Memory_H
#include <systemc.h>
#include <cstdlib>
#include <ctime>

SC_MODULE(Memory){
    //port
    sc_in_clk       iclk;
    sc_in<bool>     LD; // load/execute command
    sc_in<bool>     RW; // read high/write low
    sc_in<unsigned> X;  // X address
    sc_in<unsigned> Y;  // Y address
    sc_inout_rv<32> D;  // data
    SC_HAS_PROCESS(Memory);
    // Constructor
    Memory(sc_module_name nm) : sc_channel(nm)  {
        data = new int*[8];
        for(int i=0;i<8;i++)
            data[i] = new int[8];
        
        //print original data in Memory
        srand(time(NULL));
        for(int j=0;j<8;j++) {
            for(int i=0;i<8;i++) {
                data[j][i] = rand()%100;
            }
        }

        SC_THREAD(memory_thread);
        sensitive << iclk.neg();
        dont_initialize();
    }
    void memory_thread();
    void print_memory();

private:
    int** data;

};

#endif
