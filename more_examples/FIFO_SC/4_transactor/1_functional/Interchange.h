#ifndef Interchange_H
#define Interchange_H
#include <systemc.h>
#include "Mem_if.h"

SC_MODULE(Interchange) {
    sc_port<Mem_if> dual_data_port;
    SC_CTOR(Interchange)
    {
        reg = new int*[8];
        for(int i=0;i<8;i++)
            reg[i] = new int[8];
        SC_THREAD(interchange_thread);
    }
    void interchange_thread(void);
    void rotation(void);
    int** reg;
};

#endif
