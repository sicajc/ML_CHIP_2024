#include <systemc.h>
#include "Memory.h"
#include <iostream>
#include <iomanip>
using namespace std;

void Memory::word_read(unsigned x, unsigned y, int& d)
{
    //read data
    d = data[y][x];
}

void Memory::word_write(unsigned x, unsigned y, int d)
{
    //Memory write
    data[y][x] = d;
}

void Memory::print_memory()
{
    for(int j=7;j>=0;j--) {
        cout << "    [ ";
        for(int i=0;i<8;i++) {
            cout << setw(2) << data[j][i] << " ";
        }
        cout << "]\n";
    }
}
