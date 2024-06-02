#ifndef CORE_H
#define CORE_H

#include "systemc.h"

SC_MODULE( Core ) {
    sc_in  < bool >  rst_n;
    sc_in  < bool >  clk;
    // receive
    sc_in  < sc_lv<34> > flit_rx;	// The input channel
    sc_in  < bool > req_rx;	        // The request associated with the input channel
    sc_out < bool > ack_rx;	        // The outgoing ack signal associated with the input channel
    // transmit
    sc_out < sc_lv<34> > flit_tx;	// The output channel
    sc_out < bool > req_tx;	        // The request associated with the output channel
    sc_in  < bool > ack_tx;	        // The outgoing ack signal associated with the output channel

};

#endif