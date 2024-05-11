// receiver.h
#include "packet.h"

SC_MODULE(receiver)
{
	sc_in<packet_type> rpackin;
	sc_out<packet_type> rpackout;
	sc_out<long> dout;
	sc_in<bool> rclk;

	int framenum;
	packet_type packin, packold;
	packet_type s;
	int retry;

	void receive_data();

	// Constructor
	SC_CTOR(receiver)
	{
		SC_METHOD(receive_data);
		// SC_METHOD is used for synchronous processes
		sensitive_pos << rclk;
		framenum = 1;
		retry = 1;
	}
};
