#include "Consumer.h"
#include <cctype>
#include <systemc.h>

Consumer::Consumer(sc_module_name mname) : sc_module(mname)
{
	SC_THREAD(Main);
}

void Consumer::Main()
{
	while (true)
	{
		char c;
		const bool ALWAYS_BLOCK = false;
		auto PrintGuard = [](char c) -> char
		{ return isprint(c) and not isspace(c) ? c : '_'; };
		if (ALWAYS_BLOCK || in_->Empty())
		{
			cout << "Read: Blocking from " << sc_time_stamp();
			in_->Read(c);
			cout << " to " << sc_time_stamp() << '(' << PrintGuard(c) << ")." << endl;
		}
		else
		{
			cout << "Read: Nonblocking from " << sc_time_stamp();
			in_->Read(c);
			cout << " to " << sc_time_stamp() << '(' << PrintGuard(c) << ")." << endl;
		}
		wait(100, SC_NS);
	}
}
