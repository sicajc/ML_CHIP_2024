#include <systemc.h>
#include "Interchange.h"
#include "Memory.h"
#include <iostream>
using namespace std;

char* simulation_name = "memory read/write (PCA)";

int sc_main(int argc, char* argv[]) {

    sc_clock clk ("clk", sc_time(10,SC_NS));
    sc_time	SIM_TIME( 1600, SC_NS );

    sc_signal<bool>			LD;
    sc_signal<bool>			RW;
    sc_signal<unsigned>	    X;
    sc_signal<unsigned>	    Y;
    sc_signal_rv<32>		D;

    Interchange	iInterchange("iInterchange");
    Memory iMemory("iMemory");

    iMemory.iclk(clk);
    iMemory.LD(LD);
    iMemory.RW(RW);
    iMemory.X(X);
    iMemory.Y(Y);
    iMemory.D(D);

    iInterchange.iclk(clk);
    iInterchange.LD(LD);
    iInterchange.RW(RW);
    iInterchange.X(X);
    iInterchange.Y(Y);
    iInterchange.D(D);

    cout << "INFO: Simulating "<< simulation_name << "..." << endl;
    cout << "----- Original data in Memory: " << endl;
    iMemory.print_memory();
    sc_start(SIM_TIME);
    cout << "INFO: Complete memory write." << endl;
    cout << "----- New data in Memory: " << endl;
    iMemory.print_memory();
    return 0;
}
