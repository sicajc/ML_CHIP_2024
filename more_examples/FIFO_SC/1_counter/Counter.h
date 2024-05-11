#pragma once
#include <systemc.h>

SC_MODULE(Counter)
{
	// Port declaration
	sc_in_clk clk_;		  // postive edge triggered
	sc_in<bool> rst_;	  // true for reset
	sc_in<bool> enable_;  // true for counting
	sc_in<bool> up_down_; // ture for count up
	sc_out<int> value_;

	// Normal C++ members and functions
	void ProcessMethod(void);
	int v_;

	// Constructor declaration
	SC_CTOR(Counter)
	{							  // registration the constructor of "Counter"
		SC_METHOD(ProcessMethod); // registration process_method() as event handler
		dont_initialize();		  // method process_method() is not made runnable during initialization
		sensitive << clk_.pos();  // process_method() is triggered at positive edge of Clock
		v_ = 0;					  // give counter initial value
	}
};
