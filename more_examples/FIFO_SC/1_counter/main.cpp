#include <systemc.h>
#include "Counter.h"
#include "TestDriver.h"
#include "Display.h"

int sc_main(int argc, char** argv)
{
	// Signal declaration
	const sc_time SIM_TIME(500, SC_NS);
	sc_clock        clk("clk", sc_time(10,SC_NS));
	sc_signal<bool> rst;
	sc_signal<int>  counter_value;
	sc_signal<bool> enable;
	sc_signal<bool> up_down;

	// Instantiation and net connection
	Counter counter("counter");
	TestDriver driver("driver");
	counter.clk_(clk);
	counter.rst_(rst);
	counter.enable_(enable);
	counter.up_down_(up_down);
	counter.value_(counter_value);
	driver.clk_(clk);
	driver.rst_(rst);
	driver.enable_(enable);
	driver.up_down_(up_down);
	driver.value_(counter_value);

	// Run simulation
	sc_start(SIM_TIME);

	// Clean up
	return 0;
}
