#include <systemc.h>
#include "Fifo.h"
#include "Producer.h"
#include "Consumer.h"

int sc_main (int argc , char **argv)
{
	const int FSIZE = 10;
	Fifo fifo("fifo", FSIZE);
	Producer prod("producer");
	Consumer cons("consumer");
	prod.out_(fifo);
	cons.in_(fifo);
	/* Compare this:
	sc_signal<int> s("s"); | Fifo fifo("fifo");
	Module1 m1("m1");      | Producer prod("producer");
	Module2 m2("m2");      | Consumer cons("consumer");
	m1.out_(s);            | prod.out(fifo);
	m2.in_(s);             | cons.in(fifo);
	*/
	// Fifo implements, ReadInterface and WriteInterface
	// ==> signal: a special type implementing the interface sc_in and sc_out
	sc_start(10,SC_US);
	return 0;
}
