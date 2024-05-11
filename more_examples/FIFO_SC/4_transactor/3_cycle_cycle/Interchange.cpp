#include <systemc.h>
#include "Interchange.h"
#include <iostream>
#include <iomanip>
using namespace std;

enum operation {WRITE=false, READ=true};

void Interchange::interchange_thread(void)
{
    //Memory read
    for(int j=0;j<8;j++) {
        for(int i=0;i<8;i++) {
            cout << "INFO: " << name() << "::read starting @ " << sc_time_stamp() << " Addr (" << i << "," << j << ")" << endl;
            LD->write(true);
            RW->write(READ);
            X->write(i);
            Y->write(j);
            D->write(sc_lv<32>(SC_LOGIC_Z));
            wait();
            LD->write(false);





            reg[j][i] = D->read().to_int();
        }
    }
    cout << "INFO: Complete Memory read." << endl;
    rotation();
    //Memory write
    for(int j=0;j<8;j++) {
        for(int i=0;i<8;i++) {
            cout << "INFO: " << name() << "::write starting @ " << sc_time_stamp() << " Addr (" << i << "," << j << ")" << endl;
            wait();
            LD->write(true);
            RW->write(WRITE);
            X->write(i);
            Y->write(j);
            D->write(reg[j][i]);







        }
    }

    wait();
    LD->write(false);
    D->write(sc_lv<32>(SC_LOGIC_Z));

    for(;;)	wait();
}

void Interchange::rotation(void)
{
    for(int j=0;j<8;j++) {
        for(int i=0;i<j;i++) {
            int temp = reg[j][i];
            reg[j][i] = reg[i][j];
            reg[i][j] = temp;
        }
    }
}
