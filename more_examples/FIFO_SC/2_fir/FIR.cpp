#include "FIR.h"
#include <numeric>
#include <algorithm>

FIR::FIR(sc_module_name mname, const std::vector<int> &&coeff) : sc_module(mname),
																 cur_(0),
																 // fifo depth
																 orig_in_(32),
																 data_in_(32),
																 data_out_(32),
																 coeff_(coeff),
																 pipe_(coeff.size(), 0)
{
	fputs("filter coefficients: [ ", stdout);
	for (int x : coeff_)
	{
		printf("%d ", x);
	}
	puts("]");

	// reverse(coeff_.begin(), coeff_.end());
	// Please explain what these SC_THREAD does?
	SC_THREAD(Driver);
	SC_THREAD(FirExecution);
	SC_THREAD(Monitor);
}

void FIR::Driver()
{
	const sc_time DELAY(1, SC_NS);
	const unsigned PTS = 20;
	for (unsigned t = 0; t < PTS; t++)
	{
		const int data = (t == 10 or t == 12) ? 1 : 0;
		orig_in_.write(data);
		data_in_.write(data);
		wait(DELAY);
	}
}

void FIR::FirExecution()
{
	while (true)
	{
		data_out_.write(data_in_.read() + 100);
	}
}

void FIR::Monitor(void)
{
	for (unsigned i = 0;; i++)
	{
		const int data = orig_in_.read();
		const int result = data_out_.read();
		printf("[tap %u] %d %d\n", i, data, result);
	}
}
