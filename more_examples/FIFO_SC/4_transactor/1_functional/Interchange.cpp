#include <systemc.h>
#include "Interchange.h"
#include <iostream>
#include <iomanip>
using namespace std;

void Interchange::interchange_thread(void)
{
    dual_data_port->direct_read(reg);
    cout << "INFO: Complete memory read." << endl;
    rotation();
    dual_data_port->direct_write(reg);
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
