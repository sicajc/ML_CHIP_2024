#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include "pe.h"
#include <cstring>
#include <helperfunction.h>
using namespace std;

// The core interfaces, build lookup table here!
SC_MODULE(CORE)
{
    sc_in<bool> rst;
    sc_in<bool> clk;
    // receive
    sc_in<sc_lv<34>> flit_rx; // The input channel
    sc_in<bool> req_rx;       // The request associated with the input channel
    sc_out<bool> ack_rx;       // The outgoing ack signal associated with the input channel

    // transmit
    sc_out<sc_lv<34>> flit_tx; // The output channel
    sc_in<bool> ack_tx;       // The outgoing ack signal associated with the output channel
    sc_out<bool> req_tx;       // The request associated with the output channel

    PE pe;

    int id;

    // Trace file
    sc_trace_file *tf;

    // global variable
    int flit_counts = 0;

    void send_packet()
    {



    }


    void receive_packet()
    {





    }

    //=============================================================================
    // For File dumping & Constructor
    //=============================================================================
    SC_CTOR(CORE);

    CORE(sc_module_name name, int id, sc_trace_file *tf = nullptr) : sc_module(name), id(id)
    {
        SC_THREAD(send_packet);
        SC_THREAD(receive_packet);

        pe.init(id);

        this->id = id;
        this->tf = tf;
        sensitive << clk.pos();
        dont_initialize();

        // trace and dump the signals in a hierarchical way
        sc_trace(tf, clk, "core_" + to_string(id) + ".clk"); // the dump signal would be named as core_0.clk
        sc_trace(tf, rst, "core_" + to_string(id) + ".rst");
        sc_trace(tf, flit_rx, "core_" + to_string(id) + ".flit_rx");
        sc_trace(tf, req_rx, "core_" + to_string(id) + ".req_rx");
        sc_trace(tf, ack_rx, "core_" + to_string(id) + ".ack_rx");
        sc_trace(tf, flit_tx, "core_" + to_string(id) + ".flit_tx");
        sc_trace(tf, req_tx, "core_" + to_string(id) + ".req_tx");
        sc_trace(tf, ack_tx, "core_" + to_string(id) + ".ack_tx");
    }
};

#endif