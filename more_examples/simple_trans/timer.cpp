// timer.cpp
#define SC_USE_SC_STRING_OLD
#include "timer.h"

void timer::runtimer()
{
	while (true) // This works like a thread, the timer would runs forever
	{
		if (start)
		{
			cout << "Timer: timer start detected " << endl;
			count = 5;
			timeout = false;
			// start = false;
		}
		else
		{
			if (count > 0)
			{
				count--;
				timeout = false;
			}
			else
			{
				timeout = true;
			}
		}
		// This wait, waiting for the trigger to start the timer which is the clock signal
		// Clock signal declared in sensitive_pos << clock;
		wait();
	}
}
