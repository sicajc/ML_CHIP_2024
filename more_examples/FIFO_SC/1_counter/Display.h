#pragma once
#include <systemc.h>

SC_MODULE(Display) {
	// Port declaration
	sc_in<int> value_;

	// Constructor declaration
	SC_HAS_PROCESS(Display);
	Display(sc_module_name mn);

	// Process declaration
	void ProcessMethod(void);

	// Private data
	int old_value_;
};
