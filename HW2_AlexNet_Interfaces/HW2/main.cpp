#include "Alexnet.h"
#include "Pattern.h"
#include "Monitor.h"
#include "clockreset.h"
#include <iostream>

using namespace std;
typedef std::vector<double> Tensor1d;
typedef std::vector<std::vector<double>> Tensor2d;
typedef std::vector<std::vector<std::vector<double>>> Tensor3d;
typedef std::vector<std::vector<std::vector<std::vector<double>>>> Tensor4d;


int sc_main( int argc, char* argv[] ) {
	// Instantiation of inputs declaration signals
	sc_signal < bool > clk, rst;
	sc_signal < bool > start;
	sc_signal < double* > tensor3dImgi;
	sc_signal < double* > conv1_result;
	sc_signal < double* > fully_connected_result;
	sc_signal < bool > valid_o;

	// Instantiation of the reset, with paramter
	Reset m_Reset( "m_Reset", 10 );
	Clock m_clock( "m_clock", 5, 40 );
	ALEXNET m_Alexnet( "m_Alexnet" );
	Monitor m_Monitor( "m_Monitor" );
	Pattern m_Pattern( "m_Pattern" );

	m_Reset( rst );
	m_clock( clk );
	// m_Alexnet(  );
	// do port connection on m_Alexnet
	m_Alexnet.clk( clk );
	m_Alexnet.rst( rst );
	m_Alexnet.start( start );
	m_Alexnet.tensor3dImgi( tensor3dImgi );
	m_Alexnet.fully_connected_result( fully_connected_result);
	m_Alexnet.valid_o( valid_o );

	m_Monitor.clock( clk );
	m_Monitor.rst( rst );
	m_Monitor.start( start );
	m_Monitor.tensor3dImgi( tensor3dImgi );
	m_Monitor.fully_connected_result( fully_connected_result );
	m_Monitor.conv1_result( conv1_result );
	m_Monitor.valid_o( valid_o );

	// use port connection connecting m_Pattern
	m_Pattern.clock( clk );
	m_Pattern.rst( rst );
	m_Pattern.start( start );
	m_Pattern.tensor3dImgi( tensor3dImgi );


	// Start simulation
	sc_start( 1000, SC_NS );
	return 0;

}
