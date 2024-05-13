#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"
#include "USER_DEFINED_PARAM.h"
#include "helperfunction.h"
#include "Fifo.h"
using namespace std;

SC_MODULE(Router)
{
    sc_in<bool> rst;
    sc_in<bool> clk;

    sc_out<flit_size_t> out_flit[5];
    // Hand shake signal for output channels
    sc_out<bool> out_req[5];
    sc_in<bool> in_ack[5];

    // In channels going into the buffers, once the buffer size is full, lock the buffer
    // give large size buffers first for funcionality
    sc_in<flit_size_t> in_flit[5];
    // Only when handshaking completes, allowing the flit to be transfered.
    sc_in<bool> in_req[5];
    sc_out<bool> out_ack[5];

    int out_buf_busy[5];

    int id;
    sc_trace_file *tf;

    void route_flits()
    {

    }

    //=============================================================================
    // For File dumping & Constructor
    //=============================================================================
    SC_CTOR(Router);
    Router(sc_module_name name, int id, sc_trace_file *tf = nullptr) : sc_module(name), id(id)
    {
        SC_THREAD(route_flits);
        this->id = id;
        this->tf = tf;
        sensitive << clk.pos();
        dont_initialize();

        sc_trace(tf, clk, "R_" + std::to_string(id) + ".clk");
        sc_trace(tf, rst, "R_" + std::to_string(id) + ".rst");

        // In order to dump the signals in a hierarchical way, we need to trace the signals in the router
        // The signals cannot be dumped in array format, one must seperate them out to dump the signals
        for (int i = 0; i < 5; i++)
        {
            // I/Os
            sc_trace(tf, in_flit[i], "R_" + to_string(id) + ".in_flit_" + to_string(i));
            sc_trace(tf, in_req[i], "R_" + to_string(id) + ".in_req_" + to_string(i));
            sc_trace(tf, in_ack[i], "R_" + to_string(id) + ".in_ack_" + to_string(i));
            sc_trace(tf, out_flit[i], "R_" + to_string(id) + ".out_flit_" + to_string(i));
            sc_trace(tf, out_req[i], "R_" + to_string(id) + ".out_req_" + to_string(i));
            sc_trace(tf, out_ack[i], "R_" + to_string(id) + ".out_ack_" + to_string(i));
            // Inner signals
            sc_trace(tf, out_buf_busy[i], "R_" + to_string(id) + ".out_busy_" + to_string(i));
        }
    }
};

#endif