#include <systemc.h>
#include <iostream>

using namespace std;

SC_MODULE( Monitor ) {

	sc_in < bool > rst;
	sc_in_clk clock;
	sc_in<bool> start;
	sc_in < double* > tensor3dImgi;
	sc_in < double* > fully_connected_result;
	sc_in < double* > conv1_result;
	sc_in < bool > valid_o;


	int cycle;

	void run();

	SC_CTOR( Monitor )
	{
		cycle = 0;

		SC_THREAD( run );
		sensitive << clock.pos();
	}
};
