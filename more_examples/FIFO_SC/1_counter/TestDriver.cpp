#include "TestDriver.h"
#include <cassert>

TestDriver::TestDriver(sc_module_name mname): sc_module(mname)
{
	SC_THREAD(ProcessThread);
	// This makes wait() wait the posedge of clock
	sensitive << clk_.pos();
}

void TestDriver::ProcessThread()
{
	// initialization phase
	int i;
	enable_.write(false);
	rst_.write(false);
	up_down_.write(false);
	wait();

	// running phase
	rst_.write(true);
	wait();
	rst_.write(false);
	wait();

	enable_.write(true);
	up_down_.write(true);

	// TODO: Add count down test here
	for(int i=0; i < 20; i++) wait();
	enable_.write(false);
	for(int i=0; i < 4; i++) wait();

	// What should the value be?
	assert(value_.read() == 20);
}
