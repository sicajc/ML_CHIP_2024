#include <systemc.h>
#include "Memory.h"
#include <iostream>
#include <iomanip>
using namespace std;

enum operation {WRITE=false, READ=true};

void Memory::memory_thread()
{
    unsigned x;
    unsigned y;
    for(;;) {
        if(LD->read() == true) {
            if(RW->read() == READ) { //read 
                x = X->read();
                y = Y->read();
                D->write(data[y][x]);
            }
            else { //write
                x = X->read();
                y = Y->read();
                data[y][x] = D->read().to_int();
            }
        }
        else {
            D->write(sc_lv<32>(SC_LOGIC_Z));
        }
        wait();
    }
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

