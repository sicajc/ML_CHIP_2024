// display.cpp
#define SC_USE_SC_STRING_OLD
#include "display.h"

void display::print_data()
{
	cout << "Display: Data value received, Data = " << din.read() << endl;
}