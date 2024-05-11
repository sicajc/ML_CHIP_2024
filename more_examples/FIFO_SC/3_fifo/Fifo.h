#pragma once
#include "Interface.h"
#include <systemc.h>
#include <vector>

class Fifo : public sc_channel, public ReadInterface, public WriteInterface
{
public:
	Fifo(sc_module_name name, int size_);
	~Fifo();
	void Write(char c);
	void Read(char &c);
	void Reset();
	int NumAvailable();
	bool Full();
	bool Empty();

private:
	std::vector<char> data_;
	int head_, tail_, n_;
	sc_event write_event_, read_event_;
	int num_read_, max_used_, average_;
	sc_time last_time_;
	void ComputeStats();
};
