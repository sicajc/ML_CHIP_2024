#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include "pe.h"

// The core interfaces, build lookup table here!
SC_MODULE(Core)
{
    sc_in<bool> rst;
    sc_in<bool> clk;
    // receive
    sc_in<sc_lv<34>> flit_rx; // The input channel
    sc_in<bool> req_rx;       // The request associated with the input channel
    sc_out<bool> ack_rx;      // The outgoing ack signal associated with the input channel
    // transmit
    sc_out<sc_lv<34>> flit_tx; // The output channel
    sc_out<bool> req_tx;       // The request associated with the output channel
    sc_in<bool> ack_tx;        // The outgoing ack signal associated with the output channel

    PE pe(0);

    void flitilization()
    {
        // Receive and decompose the packet to flits from pe
        Packet* p = pe.get_packet();
        // print out the packet and their data
        cout << "Packet from PE " << p->source_id << " to PE " << p->dest_id << " is received." << endl;
        for (int i = 0; i < 5; i++)
        {
            cout << "Data " << i << ": " << p->datas[i] << endl;
        }

        // Decompose the data into flits

    }

    void send_flits()
    {
        for(;;)
        {

            wait();
        }
    }

    SC_CTOR(Core)
    {
        SC_METHOD(flitilization);
        sensitive << clk.pos();
        SC_THREAD(send_flits);
        sensitive << clk.pos();
    }
};

#endif