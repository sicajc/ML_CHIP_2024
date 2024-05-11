#include <systemc.h>
#include "Interchange.h"
#include "Memory.h"
#include <iostream>
using namespace std;

char* simulation_name = "memory read/write (untimed)";

int sc_main(int argc, char* argv[]) {
    Interchange iInterchange("iInterchange");
    Memory iMemory("iMemory");
    iInterchange.dual_data_port(iMemory);
    
    cout << "INFO: Simulating "<< simulation_name << "..." << endl;
    cout << "----- Original data in Memory: " << endl;
    iMemory.print_memory();
    sc_start();
    cout << "INFO: Complete memory write." << endl;
    cout << "----- New data in Memory: " << endl;
    iMemory.print_memory();
    return 0;
}
