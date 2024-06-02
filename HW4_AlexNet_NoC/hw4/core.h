#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include "pe.h"
#include <cstring>
#include <helperfunction.h>
#include <deque>
using namespace std;

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
    sc_in<bool> ack_tx;        // The outgoing ack signal associated with the output channel
    sc_out<bool> req_tx;       // The request associated with the output channel

    // PE pe;

    int id;

    Packet *pkt_tx, *pkt_rx;

    // Trace file
    sc_trace_file *tf;

    // global variable

    void send_packet()
    {
        int flit_counts = 0;
        int num_of_flits;

        std::deque<float> datas_q;

        int src_id;
        int dest_id;

        int done_sending_flag;


        for (;;)
        {
            if (rst.read() == true)
            {
                // reset all output ports
                req_tx.write(false);
                flit_tx.write(0);
                flit_counts = 0;
                done_sending_flag = false;
                pkt_tx == nullptr;
            }
            else if(pkt_tx == nullptr)
            {

                pkt_tx = nullptr;
                // need checking for null ptr to perform operation on pkt_tx since it is a pointer
                if (pkt_tx != nullptr)
                {

                    vector<float> datas_f = pkt_tx->datas;
                    // copy datas_f into data_q
                    for (int i = 0; i < datas_f.size(); i++)
                    {
                        datas_q.push_back(datas_f[i]);
                    }
                    num_of_flits = datas_q.size() + 1; // 1 for additional header
                    src_id = pkt_tx->source_id;
                    dest_id = pkt_tx->dest_id;
                    done_sending_flag = false;
                    cout << "Core_" << id << " get packet" << "src_id:" << src_id << ", dest_id:" << dest_id << ", num_of_flits:" << num_of_flits << endl;
                }
                else
                {
                    vector<float> datas_f;
                    num_of_flits = 0; // 1 for additional header
                    src_id = 0;
                    dest_id = 0;
                    req_tx.write(false);
                    flit_tx.write(0);
                    flit_counts = 0;
                    done_sending_flag = false;
                    pkt_tx == nullptr;
                }
            } // pkt_tx is not nullptr
            else if (flit_counts < num_of_flits)
            {
                // display
                // cout << "Core_" << id << " send packet, num_of_flits:" << num_of_flits << endl;
                // more data to send, send req to the router
                req_tx.write(true);
                if (flit_counts == 0) // header
                {
                    // cout << "Core_" << id << " send header" << endl;
                    // send the header
                    flit_size_t header = 0;
                    header.range(33, 32) = 0b10;
                    // src_id is 4 bits
                    header.range(31, 28) = src_id;
                    // dest_id is 4 bits
                    header.range(27, 24) = dest_id;
                    // rest 0
                    header.range(23, 0) = 0;
                    flit_tx.write(header);
                    flit_counts++;
                }
                else if (ack_tx.read() == true && req_tx.read() == true)
                {
                    // cout << "Core_" << id << " send data" << endl;

                    if (flit_counts == num_of_flits - 1)
                    {
                        // send the tail, dequeue the data
                        float temp;
                        temp = datas_q.front();
                        datas_q.pop_front();
                        // convert this temp data into sc_lv<32>
                        flit_size_t tail = 0;
                        tail.range(33, 32) = 0b01;
                        tail.range(31, 0) = float_to_sc_lv(temp);

                        // pop data out from the vector data
                        flit_tx.write(tail);
                    }
                    else
                    {
                        // send the body
                        float temp;
                        temp = datas_q.front();
                        datas_q.pop_front();
                        // convert this temp data into sc_lv<32>
                        flit_size_t body = 0;
                        body.range(33, 32) = 0b00;
                        body.range(31, 0) = float_to_sc_lv(temp);
                        flit_tx.write(body);
                    }

                    flit_counts++;
                    // cout << "Core_" << id << " send data cnt:" << flit_counts << endl;
                }
            }
            else if (done_sending_flag == false && ack_tx.read() == true)
            {
                // no more data to send, stop sending request to router
                req_tx.write(false);
                flit_tx.write(0);
                // display done sending signal
                // cout << "Core_" << id << " done sending" << endl;
                done_sending_flag = true;
                pkt_tx = nullptr;
                //reset
                num_of_flits = 0; // 1 for additional header
                src_id = 0;
                dest_id = 0;
                flit_counts = 0;
                done_sending_flag = false;
                pkt_tx == nullptr;
            }

            wait();
        }
    }

    void receive_packet()
    {
        for (;;)
        {
            flit_size_t flit = flit_rx.read();
            // ack receive
            if (rst.read() == true)
            {
                ack_rx.write(false);
            }
            else if (ack_rx.read() == true)
            {
                ack_rx.write(false);
            }
            else if (req_rx.read() == true)
            {
               ack_rx.write(true);
            }
            else
            {
                ack_rx.write(false);
            }

            if (req_rx.read() == true && ack_rx.read() == true)
            {
                // cout << "Core_" << id << " receive packet" << endl;
                // receive the packet

                // display flit read in
                // cout << "Core_" << id << " receive flit:" << flit << endl;
                // header

                // send ack to the router, oscilates the ack_rx
                if (flit.range(33, 32) == 0b10) // header
                {
                    pkt_rx = new Packet;

                    if (pkt_rx == nullptr)
                        cout << "Core_" << id << " ERROR: header received while receiving packet" << endl;
                    else
                    {
                        pkt_rx->source_id = flit.range(31, 28).to_uint();
                        pkt_rx->dest_id = flit.range(27, 24).to_uint();
                        //cout << "Core_" << id << " receive header, src_id:" << pkt_rx->source_id << ", dest_id:" << pkt_rx->dest_id << endl;
                    }
                }
                else if (flit.range(33, 32) == 0b01) // tail received
                {
                    // tail
                    float temp = sc_lv_to_float(flit.range(31, 0));
                    if (pkt_rx != nullptr)
                        pkt_rx->datas.push_back(temp);
                    cout << "Core_" << id << " receive tail, data:" << temp << endl;

                    // display the whole pckt_rx->datas
                    //cout << "pkt_rx->source_id:" << pkt_rx->source_id << ", pkt_rx->dest_id:" << pkt_rx->dest_id << endl;

                    // for (int i = 0; i < pkt_rx->datas.size(); i++)
                    // {
                    //     cout << "Core_" << id << " receive data:" << pkt_rx->datas[i] << endl;
                    // }

                    // check the packet
                    // pe.check_packet(pkt_rx);
                }
                else
                {
                    // body
                    float temp = sc_lv_to_float(flit.range(31, 0));
                    if (pkt_rx != nullptr)
                        pkt_rx->datas.push_back(temp);
                    // cout << "Core_" << id << " receive body, data:" << temp << endl;
                }
            }

            wait();
        }
    }

    //=============================================================================
    // For File dumping & Constructor
    //=============================================================================
    SC_CTOR(Core);

    Core(sc_module_name name, int id, sc_trace_file *tf = nullptr) : sc_module(name), id(id)
    {
        // constructor
        // pe.init(id);
        this->id = id;
        this->tf = tf;

        SC_THREAD(send_packet);
        dont_initialize();
        sensitive << clk.pos();

        SC_THREAD(receive_packet);
        dont_initialize();
        sensitive << clk.pos();

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