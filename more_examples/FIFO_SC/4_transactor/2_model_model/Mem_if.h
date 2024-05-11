#ifndef Mem_if_H
#define Mem_if_H
#include <systemc.h>

struct Mem_if : public sc_interface
{
    virtual void word_read(unsigned x, unsigned y, int &d) = 0;
    virtual void word_write(unsigned x, unsigned y, int d) = 0;
};

#endif
