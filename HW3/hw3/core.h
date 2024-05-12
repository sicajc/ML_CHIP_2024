#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include "pe.h"
#include <cstring>
#include <helperfunction.h>

// The core interfaces, build lookup table here!
SC_MODULE(CORE)
{
    sc_in<bool> rst;
    sc_in<bool> clk;
    // receive
    sc_in<sc_lv<34>> flit_rx; // The input channel
    sc_in<bool> req_rx;       // The request associated with the input channel
    sc_in<bool> ack_tx;        // The outgoing ack signal associated with the output channel

    // transmit
    sc_out<bool> ack_rx;      // The outgoing ack signal associated with the input channel
    sc_out<sc_lv<34>> flit_tx; // The output channel
    sc_out<bool> req_tx;       // The request associated with the output channel

    PE pe;

    // global variable
    int flit_counts = 0;

    void send_flits()
    {
        // how to read out the local variable data from flitilization function
        pe.init(0);
        // Receive and decompose the packet to flits from pe
        Packet *p = pe.get_packet();

        // number of slots in datas
        flit_counts = p->datas.size();

        // print out the packet and their data
        // cout << "Packet from PE " << p->source_id << " to PE " << p->dest_id << " is received." << endl;
        // for (int i = 0; i < 5; i++)
        // {
        //     cout << "Data " << i << ": " << p->datas[i] << endl;
        // }

        sc_lv<34> data[flit_counts + 1];

        // Decompose the data into flits
        // convert all data into sc_lv<32> type
        // determine the size of data vector, 1 is header others are body flit and tail flit

        // Set the Header
        data[0].range(33, 32) = 2;

        // Use algorithmic routing to determine the next hop, thus store
        // store (src_id,dst_id,x,y) into the header flit, x,y is the directional vector to the destination
        // src_id 4 bits,dst_id 4 bits, x 4 bits, y 4 bits
        // convert integer source_id into 4 bits value
        data[0].range(31, 28) = p->source_id;
        // convert integer dest_id into 4 bits value
        data[0].range(27, 24) = p->dest_id;
        // convert integer x into 5 bits signed value, store it to data
        // calculation the x displacement vector
        int x = p->dest_id % 4 - p->source_id % 4;
        data[0].range(23, 19) = x;

        // print out the x displacement vector from data[0]
        // cout << "X displacement vector: " << data[0].range(23, 19) << endl;

        // convert integer y into 5 bits value
        // calculation the y displacement vector
        int y = p->dest_id / 4 - p->source_id / 4;
        data[0].range(18, 14) = y;

        // print out the y displacement vector from data[0]
        // cout << "Y displacement vector: " << data[0].range(18, 14) << endl;
        // fill other field with 0

        int dst_x = p->dest_id % 4;
        int dst_y = p->dest_id / 4;

        // print out the destination x and y
        // cout << "Destination X: " << dst_x << " Destination Y: " << dst_y << endl;

        //place dst_x and dst_y into the header flit
        data[0].range(13, 10) = dst_x;
        data[0].range(9, 6) = dst_y;

        data[0].range(5, 0) = 0;
        // print out the header information
        // cout << "====================================================="<<endl;
        // cout << "Header flit: " << data[0] << endl;
        // cout << "Source ID: " << data[0].range(31, 28).to_uint() << endl;
        // cout << "Destination ID: " << data[0].range(27, 24).to_uint() << endl;
        // cout << "Signed X displacement vector: " << data[0].range(23, 19).to_int() << endl;
        // cout << "Signed Y displacement vector: " << data[0].range(18, 14).to_int() << endl;
        // cout << "Destination X: " << data[0].range(13, 10).to_int() << endl;
        // cout << "Destination Y: " << data[0].range(9, 6).to_int() << endl;
        // cout << "====================================================="<<endl;

        for (int i = 0; i < flit_counts; i++)
        {
            // convert float to sc_lv<32> then concatenate with 2 bits flit type
            sc_lv<32> flit_data = float_to_sc_lv(p->datas[i]);
            // set flit type
            if (i == flit_counts)
            {
                // Tail flit
                data[i + 1] = flit_data.range(31, 0);
                data[i + 1].range(33, 32) = 1;
            }
            else
            {
                // Body flit
                data[i + 1] = flit_data.range(31, 0);
                data[i + 1].range(33, 32) = 0;
            }
        }

        // display 5 of these flits
        // for (int i = 0; i < 5; i++)
        // {
        //     cout << "Data " << i << " in flit: " << data[i] << endl;
        // }

        for (;;)
        {
            // sends flits out 1 by 1 only when handshaked
            for (int i = 0; i < (flit_counts + 1); i++)
            {
                // send flit
                sc_lv<34> flit_to_write = data[i];

                flit_tx.write(flit_to_write);
                req_tx.write(true);

                wait();

                // wait for ack signal and the handshake
                while (!(ack_tx.read() && req_tx.read()))
                {
                    wait();
                }

                // display the flit that is sent
                cout << "Flit " << i << " sent: " << flit_tx << endl;
                // display ack and req
                cout << "Ack: " << ack_tx.read() << " Req: " << req_tx << endl;

                wait();
            }
            req_tx.write(false);

            // print flit done only once in the outer for loop
            cout << "Flit sent done!" << endl;
            wait();
        }
    }

    SC_CTOR(CORE)
    {
        SC_THREAD(send_flits);
        sensitive << clk.pos();

        // Initialize flit_tx and req_tx
        flit_tx.initialize(0);
        req_tx.initialize(false);
    }
};

#endif