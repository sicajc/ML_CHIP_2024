#ifndef Memory_H
#define Memory_H
#include <systemc.h>
#include <cstdlib>
#include <ctime>
#include "Mem_if.h"

struct Memory
    : public sc_channel
    , public Mem_if
{
    SC_HAS_PROCESS(Memory);
    // Constructors
    Memory(sc_module_name _name) 
        :sc_channel(_name)
    {
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
    }
    // User methods
    void direct_read(int** block);
    void direct_write(int** block);
    void print_memory();
private:
    int** data;
};

#endif
