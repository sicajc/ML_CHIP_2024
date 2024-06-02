#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "systemc.h"
using namespace std;

SC_MODULE(Controller)
{
    sc_in<bool> rst;
    sc_in<bool> clk;

    // to ROM
    sc_out<int> layer_id;       // '0' means input data
    sc_out<bool> layer_id_type; // '0' means weight, '1' means bias (for layer_id == 0, we don't care this signal)
    sc_out<bool> layer_id_valid;

    // from ROM
    sc_in<float> data;
    sc_in<bool> data_valid;

    // to router0
    sc_out<sc_lv<34>> flit_tx;
    sc_out<bool> req_tx;
    sc_in<bool> ack_tx;

    // from router0
    sc_in<sc_lv<34>> flit_rx;
    sc_in<bool> req_rx;
    sc_out<bool> ack_rx;

    // Trace file
    sc_trace_file *tf;


    void run()
    {
        for (;;)
        {

            wait();
        }
    }

    //=============================================================================
    // For File dumping & Constructor
    //=============================================================================
    SC_CTOR(Controller);

    Controller(sc_module_name name, sc_trace_file *tf = nullptr) : sc_module(name)
    {
        // Constructor

        // trace signals
        sc_trace(tf, rst,   "m_controller.rst");
        sc_trace(tf, clk,   "m_controller.clk");

        sc_trace(tf, layer_id,   "m_controller.layer_id");
        sc_trace(tf, layer_id_type,   "m_controller.layer_id_type");
        sc_trace(tf, layer_id_valid,   "m_controller.layer_id_valid");

        sc_trace(tf, data,   "m_controller.data");
        sc_trace(tf, data_valid,   "m_controller.data_valid");

        sc_trace(tf, flit_tx,   "m_controller.flit_tx");
        sc_trace(tf, req_tx,   "m_controller.req_tx");
        sc_trace(tf, ack_tx,   "m_controller.ack_tx");

        sc_trace(tf, flit_rx,   "m_controller.flit_rx");
        sc_trace(tf, req_rx,   "m_controller.req_rx");
        sc_trace(tf, ack_rx,   "m_controller.ack_rx");
    }
};
#endif