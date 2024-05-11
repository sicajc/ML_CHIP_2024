#pragma once
#include <systemc.h>

SC_MODULE(TestDriver) {
	// Port declaration
	sc_in_clk    clk_;
	sc_out<bool> rst_;
	sc_out<bool> enable_;
	sc_out<bool> up_down_;
	sc_in<int>   value_;

	// Constructor declaration
	SC_HAS_PROCESS(TestDriver);
	TestDriver(sc_module_name mname);

	// Process declaration
	void ProcessThread( void );
};
