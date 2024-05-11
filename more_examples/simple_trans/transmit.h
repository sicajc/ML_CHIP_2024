// transmit.h
#include "packet.h"

SC_MODULE(transmit)
{
	// Declartion of in out ports
	sc_in<packet_type> tpackin;
	sc_in<bool> timeout;
	sc_out<packet_type> tpackout;
	sc_out<bool> start_timer;
	sc_in<bool> clock;

	// Inner variables or components
	int buffer;
	int framenum;
	packet_type packin, tpackold;
	packet_type s;
	int retry;
	bool start;

	// Functions
	void send_data();		// start sending out data
	int get_data_fromApp(); // Generation of data

	// Constructor
	SC_CTOR(transmit)
	{
		SC_METHOD(send_data);
		// The event trigers as timeout changes or at posedge clk
		sensitive << timeout;
		sensitive_pos << clock;
		framenum = 1;
		retry = 0;
		start = false;
		buffer = get_data_fromApp();
	}
};
