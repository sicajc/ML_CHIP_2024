// display.h
#include "packet.h"

SC_MODULE(display)
{
	sc_in<long> din;

	void print_data();
	// Constructor
	SC_CTOR(display)
	{
		// Uses method since this should be model like a combinational
		// circuit which always print value out.
		SC_METHOD(print_data);
		sensitive << din;
	}
};
