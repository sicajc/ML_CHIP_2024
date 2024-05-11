#pragma once
#include "Interface.h"
#include <systemc.h>

class Consumer: public sc_module {
public:
	sc_port<ReadInterface> in_;
	SC_HAS_PROCESS(Consumer);
	Consumer(sc_module_name mname);
	void Main();
};
