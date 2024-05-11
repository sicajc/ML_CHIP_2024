// main.cpp
#define SC_USE_SC_STRING_OLD
#include "packet.h"
#include "timer.h"
#include "transmit.h"
#include "noisybus.h"
#include "receiver.h"
#include "display.h"

int sc_main(int argc, char* argv[]) {
	// Define type packet signals as main data type
	sc_signal<packet_type>	PACKET1, PACKET2,
			PACKET3, PACKET4;
	sc_signal<long> DOUT;
	sc_signal<bool> TIMEOUT, START;

	sc_clock CLOCK("clock", 20);	// transmit clock
	sc_clock RCLK("rclk", 15);		// receiver clock

	// Module instantiation
	transmit t1("transimit");	// transmit instantiation
	t1.tpackin(PACKET2);	// port to signal binding
	t1.timeout(TIMEOUT);
	t1.tpackout(PACKET1);
	t1.start_timer(START);
	t1.clock(CLOCK);	// clocking

	noisybus n1("noisybus");	// noisybus instantiation
	n1.tpackin(PACKET1);	// connect to transmit
	n1.rpackin(PACKET3);	// port to signal binding
	n1.tpackout(PACKET2);	// connect to transmit
	n1.rpackout(PACKET4);	// port to signal binding

	receiver r1("receiver");	// receiver instantiation
	r1.rpackin(PACKET4);	// connect to noisybus
	r1.rpackout(PACKET3);	// connect to noisybus
	r1.dout(DOUT);	// port to signal binding
	r1.rclk(RCLK);	// clocking
	display d1("display");	// display instantiation
	d1 << DOUT;	// signal to port connection

	timer tm1("timer");	// timer instantiation
	tm1 << START << TIMEOUT << CLOCK;//.signal();
			// signal to port connections


	// tracing:
	// trace file creation, with VCD type output
	sc_trace_file *tf = sc_create_vcd_trace_file("simplex");
	// External signals
	sc_trace(tf, CLOCK, "clock");
	sc_trace(tf, TIMEOUT, "timeout");
	sc_trace(tf, START, "start");
	sc_trace(tf, PACKET1, "packet1");
	sc_trace(tf, PACKET2, "packet2");
	sc_trace(tf, PACKET3, "packet3");
	sc_trace(tf, PACKET4, "packet4");
	sc_trace(tf, DOUT, "dout");
	sc_trace(tf, tm1.count, "count");

	sc_start(10000, SC_NS);	// simulate for 10000 time steps
			// default is ps

	sc_close_vcd_trace_file(tf);

	getchar();

	return(0);
}
