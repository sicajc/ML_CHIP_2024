#ifndef Mem_if_H
#define Mem_if_H


#include <systemc.h>

struct Mem_if: public sc_interface {
    virtual void direct_read(int** block) = 0;
    virtual void direct_write(int** block) = 0;
};

#endif
