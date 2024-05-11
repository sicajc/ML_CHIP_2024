#include <systemc.h>
#include <iostream>
#define CYCLE 30

using namespace std;

typedef std::vector<std::vector<std::vector<double>>> Tensor3d;

SC_MODULE( Pattern ) {

	sc_in < bool > rst;
	sc_in_clk clock;
	sc_out < bool > start;
	sc_out < double* > tensor3dImgi;

	sc_uint <32> cycle;
	sc_uint <3> temp_uint;

	void run();

	SC_CTOR( Pattern )
	{
		temp_uint = 0;
		cycle = 0;
		SC_METHOD( run );
		sensitive << clock.neg();
	}
};
