// transmit.cpp
#define SC_USE_SC_STRING_OLD
#include "transmit.h"
int transmit::get_data_fromApp()
{
	int result;
	result = rand();
	cout << "Generate: Sending Data Value = " << result << "\n";
	return result;
}
void transmit::send_data()
{
	if (timeout)
	{
		// Note this is the packet data type
		s.info = buffer;
		s.seq = framenum;
		s.retry = retry;
		retry++;
		tpackout = s;
		start_timer = true;
		cout << "Transmit:Sending packet no. " << s.seq << endl;
	}
	else
	{
		packin = tpackin;
		start_timer = false;
		if (!(packin == tpackold))
		{
			if (packin.seq == framenum)
			{
				buffer = get_data_fromApp();
				framenum++;
				retry = 0;
			}
			tpackold = tpackin;
			s.info = buffer;
			s.seq = framenum;
			s.retry = retry;
			retry++;
			tpackout = s;
			// start_timer = false;
			cout << "Transmit:Sending packet no. " << s.seq << endl;
		}
	}
}
