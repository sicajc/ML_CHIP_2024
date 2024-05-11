#define SC_INCLUDE_FX
#include <systemc.h>
#include <iostream>
#define CYCLE 15

using namespace std;

SC_MODULE(Pattern)
{

	sc_in<bool> rst;
	sc_in_clk clock;
	sc_vector<sc_out<sc_fixed_fast<40, 17>>> image{"image", 150528};

	sc_uint<32> cycle;
	sc_uint<3> temp_uint;

	sc_out<bool> in_valid;

	void run();

	SC_CTOR(Pattern)
	{
		temp_uint = 0;
		cycle = 0;
		SC_METHOD(run);
		sensitive << clock.neg();
	}
};
