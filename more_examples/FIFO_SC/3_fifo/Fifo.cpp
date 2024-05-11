#include "Fifo.h"
#include <algorithm>
#include <systemc.h>

Fifo::Fifo(sc_module_name mname, int size):
	sc_channel(mname), data_(size),
	head_(0), tail_(0), n_(0),
	num_read_(0), max_used_(0), average_(0),
	last_time_(SC_ZERO_TIME) {}

Fifo::~Fifo()
{
	cout << endl << "Fifo size is: " << data_.size() << endl;
	cout << "Average Fifo fill depth: " << double(average_) / num_read_ << endl;
	cout << "Maximum Fifo fill depth: " << max_used_ << endl;
	cout << "Average transfer time per character: " << last_time_ / num_read_ << endl;
	cout << "Total characters transferred: " << num_read_ << endl;
	cout << "Total time: " << last_time_ << endl;
}

void Fifo::Write(char c)
{
}

void Fifo::Read(char &c)
{
	last_time_ = sc_time_stamp();
	if (n_ == 0) {
		wait(write_event_);
	}
	ComputeStats();
	c = data_[head_];
	head_++;
	n_--;
	if (head_ == data_.size()) {
		head_ = 0;
	}
	read_event_.notify();
}

void Fifo::Reset()
{
	head_ = tail_ = n_ = 0;
}

int Fifo::NumAvailable()
{
	return n_;
}

bool Fifo::Full(){
}

bool Fifo::Empty(){
}

void Fifo::ComputeStats()
{
	average_ += n_;
	max_used_ = std::max(n_, max_used_);
	++num_read_;
}
