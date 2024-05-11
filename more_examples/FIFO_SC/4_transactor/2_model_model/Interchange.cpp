#include <systemc.h>
#include "Interchange.h"
#include <iostream>
#include <iomanip>
using namespace std;

void Interchange::interchange_thread(void)
{
    // Memory read
    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            dual_data_port->word_read(i, j, reg[j][i]);
        }
    }
    cout << "INFO: Complete Memory read." << endl;
    rotation();
    // Memory write
    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            dual_data_port->word_write(i, j, reg[j][i]);
        }
    }
}

void Interchange::rotation(void)
{
    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < j; i++)
        {
            int temp = reg[j][i];
            reg[j][i] = reg[i][j];
            reg[i][j] = temp;
        }
    }
}
