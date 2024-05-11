#include <systemc.h>
#include "Memory.h"
#include <iostream>
#include <iomanip>
using namespace std;

void Memory::direct_read(int** block)
{
    //read data
    for(int j=0;j<8;j++) {
        for(int i=0;i<8;i++) {
            block[j][i] = data[j][i];
        }
    }
}

void Memory::direct_write(int** block)
{
    //Memory write
    for(int j=0;j<8;j++) {
        for(int i=0;i<8;i++) {
            data[j][i] = block[j][i];
        }
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
