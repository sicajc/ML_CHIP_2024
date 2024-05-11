#pragma once
#include <systemc.h>

class ReadInterface : virtual public sc_interface
{
public:
    virtual void Read(char &) = 0;
    virtual int NumAvailable() = 0;
    virtual bool Empty() = 0;
};

class WriteInterface : virtual public sc_interface
{
public:
    virtual void Write(char) = 0;
    virtual void Reset() = 0;
    virtual bool Full() = 0;
};
