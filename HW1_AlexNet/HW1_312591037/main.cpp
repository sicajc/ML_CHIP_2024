#include "Alexnet.h"
#include "Pattern.h"
#include "Monitor.h"
#include "clockreset.h"
#include <iostream>

using namespace std;

int sc_main( int argc, char* argv[] ) {
	// Instantiation of inputs declaration signals
	sc_signal < bool > clk, rst;
	sc_signal < sc_uint<4> > A, B; // Unsigned integers with 4 bits
	sc_signal < sc_uint<8> > C;    // Unsigned integers with 8 bits
	sc_signal < sc_uint<9> > Y;	   // Unsigned integers with 9 bits

	// Instantiation of the reset, with paramter
	Reset m_Reset( "m_Reset", 10 );
	Clock m_clock( "m_clock", 5, 40 );
	ALEXNET m_Alexnet( "m_Alexnet" );
	Monitor m_Monitor( "m_Monitor" );
	Pattern m_Pattern( "m_Pattern" );

	m_Reset( rst );
	m_clock( clk );
	m_Alexnet( A, B, Y );
	m_Monitor( rst, clk, A, B, C, Y );
	m_Pattern( rst, clk, A, B, C );

	sc_start( 500, SC_NS );
	return 0;

}
