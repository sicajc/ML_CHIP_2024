// receiver.cpp
#define SC_USE_SC_STRING_OLD
#include "receiver.h"

void receiver::receive_data()
{
	packin = rpackin;
	if (packin == packold) // Check if data received is the same as the previous one
		return;
	cout << "Receiver: got packet no. = " << packin.seq << endl;
	if (packin.seq == framenum)
	{
		dout = packin.info;
		framenum++;
		retry++;
		s.retry = retry;
		s.seq = framenum - 1;
		rpackout = s;
	}
	packold = packin;
}
