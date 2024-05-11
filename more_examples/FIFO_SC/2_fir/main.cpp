#include "FIR.h"
#include <cstdio>
#include <systemc.h>

int sc_main(int argc, char **argv)
{
	FIR fir("fir", {1, 3, 2, 0, 5});
	sc_start();
	return 0;
}
