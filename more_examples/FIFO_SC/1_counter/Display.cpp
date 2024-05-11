#include "Display.h"
#include <iostream>
using std::cout;
using std::endl;

Display::Display(sc_module_name mname) : sc_module(mname), old_value_(0)
{
	// TODO
}

void Display::ProcessMethod()
{
	if (old_value_ != value_.read())
	{
		cout << "Updated to " << value_.read() << " at " << sc_time_stamp() << endl;
		old_value_ = value_.read();
	}
	else
	{
		// TODO: This is never called, why?
		cout << "Hold old value" << endl;
	}
}
