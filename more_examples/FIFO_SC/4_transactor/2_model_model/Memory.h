#ifndef Memory_H
#define Memory_H
#include "Mem_if.h"
#include <cstdlib>
#include <ctime>


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
        
        srand(time(NULL));
        for(int j=0;j<8;j++) {
            for(int i=0;i<8;i++) {
                data[j][i] = rand()%100;
            }
        }
    }

    // User methods
    void word_read(unsigned x, unsigned y, int& d);
    void word_write(unsigned x, unsigned y, int d);
    void print_memory();

private:
    int** data;
};

#endif
