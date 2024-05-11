#pragma once
#include "Interface.h"
#include <systemc.h>

class Producer: public sc_module {
public:
	sc_port<WriteInterface> out_;
	SC_HAS_PROCESS(Producer);
	Producer(sc_module_name mname);
	void Main();
};
