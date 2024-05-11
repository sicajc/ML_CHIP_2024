// noisybus.cpp
#define SC_USE_SC_STRING_OLD
#include "noisybus.h"

void noisybus::receive_data()
{
	int i;
	packin = tpackin;
	cout << "Noisybus: Received packet seq no. = " << packin.seq << endl;
	i = rand();
	packout = packin;
	cout << "Noisybus: Random number = " << i << endl;
	// Within this certain range, the packet is dropped, modeling a noisy bus channel.
	if ((i > 1000) && (i < 5000))
	{
		packout.seq = 0;
	}
	rpackout = packout;
}

void noisybus::send_ack()
{
	int i;
	ackin = rpackin;
	cout << "Noisybus: Received Ack for packet = " << ackin.seq << endl;
	i = rand();
	ackout = ackin;
	// A random chance of dropping the packet
	if ((i > 10) && (i < 500))
	{
		ackout.seq = 0;
	}
	tpackout = ackout;
}
