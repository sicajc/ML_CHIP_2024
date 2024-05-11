#include "Producer.h"
#include <systemc.h>
#include <random>
#include <algorithm>

Producer::Producer(sc_module_name mname) : sc_module(mname)
{
	SC_THREAD(Main);
}

void Producer::Main()
{
	const char *str = "Hi, Monika here!\n" "Welcome to the Literature Club!\n";
	const int total = 100;
	std::default_random_engine gen;
	std::uniform_int_distribution<int> dist(1,19);

	for (int i = 0; i < total; ++i) {
		for (int j = 0, si = 0, jmax = std::min(dist(gen), total-i); j < jmax; ++j) {
			out_->Write(str[si]);
			si++;
			if (str[si] == 0) {
				si = 0;
			}
		}
		wait(1000, SC_NS);
	}
}

