// timer.h
#include "systemc.h"

SC_MODULE(timer)
{
	sc_in<bool> start;
	sc_out<bool> timeout;
	sc_in<bool> clock;

	int count;

	void runtimer();

	// Constructor
	SC_CTOR(timer)
	{
		// This thread runs forever, triggered by the clock and start signals.
		SC_THREAD(runtimer);
		sensitive_pos << clock;
		sensitive << start;
		count = 0;
	}
};
