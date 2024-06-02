#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "pe.h"
#include <deque>
#include "systemc.h"
using namespace std;

SC_MODULE(Controller)
{
    sc_in<bool> rst;
    sc_in<bool> clk;

    // to ROM
    sc_out<int>  layer_id;       // '0' means input data
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
    sc_signal<sc_lv<32>> data_received;

    int pe_cnt_tx,state;
    int layer_id_cnt;
    int dst_id,src_id;

    Packet *pkt_rx;

    sc_signal<sc_lv<32>> data_float;

    void run()
    {
        for (;;)
        {
            if(rst.read())
            {

                layer_id.write(0);
                layer_id_type.write(0);
                layer_id_valid.write(0);

                flit_tx.write(0);
                req_tx.write(0);

                ack_rx.write(0);

                // Initial Idle state
                state = 0;
                layer_id_cnt = 1;
                dst_id = 0;
                src_id = 0;
                dst_id = 0;
                data_float = 3;
            }
            else
            {

                switch(state)
                {
                    // Receive weights,biases & img then send the datas to desired router
                    case(0):
                    {
                        std::deque<sc_lv<32>> weights_q;
                        std::deque<sc_lv<32>> biases_q;
                        std::deque<sc_lv<32>> img_q;

                        int num_of_data = 0;
                        int weights_size = 0;
                        int bias_size = 0;

                        int first_data_f = 0;

                        // Send req signals to ROM to call for weights
                        layer_id.write(layer_id_cnt);
                        layer_id_type.write(0);
                        layer_id_valid.write(1);

                        // Wait for data valid signal
                        wait();
                        layer_id_valid = 0;

                        while(data_valid.read()!=1) {
                            wait();
                        }

                        //Start receiving weights
                        while(data_valid.read()==1)
                        {
                            // Read weights and receive the weights
                            float weights_float;
                            sc_lv<32> weights_sc_lv;
                            weights_float = data.read();

                            data_received = float_to_sc_lv(weights_float);

                            if(first_data_f == 1)
                                weights_q.push_back(data_received);

                            num_of_data++;
                            first_data_f = 1;
                            wait();
                        }

                        // Get the last value
                        // Read weights and receive the weights
                        float weights_float;
                        sc_lv<32> weights_sc_lv;
                        weights_float = data.read();

                        weights_q.push_back(data_received);

                        data_received = float_to_sc_lv(weights_float);

                        cout << "Received weights of layer" << layer_id_cnt << " with " << num_of_data << " data" << endl;

                        // Send req signals to ROM to call for bias
                        layer_id_type.write(1);
                        layer_id_valid.write(1);

                        weights_size = num_of_data;
                        data_received = 0;
                        num_of_data = 0;
                        first_data_f = 0;

                        wait();
                        layer_id_valid.write(0);

                        // Wait for data valid signal
                        while(data_valid.read()!=1) wait();

                        //Start receiving biases
                        while(data_valid.read()==1)
                        {
                            // float
                            float biases_float;
                            sc_lv<32> biases_sc_lv;

                            biases_float = data.read();
                            data_received = float_to_sc_lv(biases_float);

                            // Read biases and receive the biases
                            if(first_data_f == 1)
                                biases_q.push_back(data_received);

                            first_data_f = 1;
                            num_of_data++;
                            wait();
                        }

                        // float
                        float biases_float;
                        sc_lv<32> biases_sc_lv;

                        biases_float = data.read();
                        data_received = float_to_sc_lv(biases_float);
                        biases_q.push_back(data_received);


                        cout << "Received biases of layer" <<" with " << num_of_data << " data" << endl;
                        bias_size = num_of_data;
                        data_received = 0;
                        num_of_data = 0;


                        if(layer_id_cnt == 1)
                        {
                            //Receive the img data
                            // Send req signals to ROM to call for weights
                            layer_id.write(0);
                            layer_id_type.write(0);
                            layer_id_valid.write(1);

                            // Wait for data valid signal
                            wait();
                            layer_id_valid = 0;

                            while(data_valid.read()!=1) {
                                wait();
                            }

                            //Start receiving imgs
                            while(data_valid.read()==1)
                            {
                                // Read weights and receive the weights
                                float img_float;
                                sc_lv<32> img_sc_lv;
                                img_float = data.read();

                                data_received = float_to_sc_lv(img_float);

                                if(first_data_f == 1)
                                    img_q.push_back(data_received);

                                num_of_data++;
                                first_data_f = 1;
                                wait();
                            }

                            // Get the last value
                            // Read weights and receive the weights
                            float img_float;
                            sc_lv<32> img_sc_lv;
                            img_float = data.read();

                            img_q.push_back(data_received);

                            data_received = float_to_sc_lv(weights_float);

                            cout << "Received Imgs with number of "  << num_of_data << " data" << endl;
                        }

                        // Sends packets to Routers
                        // Specify router to send
                        switch(layer_id_cnt){
                            case(1): dst_id = 1; break;
                            case(2): dst_id = 2; break;
                            case(3): dst_id = 3; break;
                            case(4): dst_id = 7; break;
                            case(5): dst_id = 6; break;
                            case(6): dst_id = 5; break;
                            case(7): dst_id = 4; break;
                            case(8): dst_id = 8; break;
                            default: dst_id = 0; break;
                        }

                        int num_of_pkt = 2;
                        if(layer_id_cnt == 1) num_of_pkt = 3;

                        // Packetlize 0 sends weights, 1 sends biases
                        for(int send_cnt = 0;send_cnt<num_of_pkt;send_cnt++)
                        {
                            // Sending value
                            std::deque<sc_lv<32>> datas_q;
                            int packet_type;
                            int packet_size;

                            // First send weights then biases
                            if(send_cnt == 0)
                            {
                                // 1 is weight
                                packet_type = 1;
                                datas_q = weights_q;
                                packet_size = weights_q.size();
                            }
                            else if(send_cnt == 1)
                            {
                                // 0 is bias
                                packet_type = 0;
                                datas_q = biases_q;
                                packet_size = biases_q.size();
                            }
                            else
                            {
                                // 2 is img
                                packet_type = 2;
                                datas_q = img_q;
                                packet_size = img_q.size();
                            }

                            // cout << "Packet size: " << packet_size<<std::endl;

                            int flit_counts = 0;
                            // Send to the router that needs the data
                            while(flit_counts < packet_size+1)
                            {
                                // Send the request to router
                                req_tx.write(true);

                                // Send header
                                if(flit_counts == 0)
                                {
                                    cout << "Controller Send header" << endl;
                                    // send the header
                                    flit_size_t header = 0;
                                    header.range(33, 32) = 0b10;
                                    // src_id is 4 bits
                                    header.range(31, 28) = src_id;
                                    // dest_id is 4 bits
                                    header.range(27, 24) = dst_id;
                                    // packet_type is 2 bits
                                    header.range(23, 22) = packet_type;
                                    //rest 0
                                    header.range(21, 0) = 0;

                                    flit_tx.write(header);
                                    flit_counts++;
                                }
                                else if(ack_tx.read() == true && req_tx.read() == true)
                                {
                                    if(flit_counts == packet_size)
                                    {
                                        // send the tail, dequeue the data
                                        cout << "Controller Sending tails" <<endl;
                                        sc_lv<32> temp;

                                        // There is no more data = =
                                        temp = datas_q.front();
                                        datas_q.pop_front();
                                        // convert this temp data into sc_lv<32>
                                        flit_size_t tail = 0;
                                        tail.range(33, 32) = 0b01;
                                        tail.range(31, 0) = temp;

                                        // pop data out from the vector data
                                        flit_tx.write(tail);
                                        flit_counts++;
                                    }
                                    else
                                    {
                                        // send the body
                                        sc_lv<32> temp;
                                        temp = datas_q.front();
                                        datas_q.pop_front();

                                        // if(flit_counts == 1 || flit_counts == 2)
                                        //     cout << "Flit values: "<< temp << endl;

                                        // convert this temp data into sc_lv<32>
                                        flit_size_t body = 0;
                                        body.range(33, 32) = 0b00;
                                        body.range(31, 0) = temp;
                                        flit_tx.write(body);
                                    }
                                    flit_counts++;
                                }
                                wait();
                            }

                            while(ack_tx.read() != true || req_tx.read() != true)
                            {
                                wait();
                            }

                            req_tx.write(0);
                            flit_tx.write(0);
                            wait();
                        }

                        layer_id_cnt++ ;

                        if(layer_id_cnt == 1)
                        {
                            state = 1;
                        }

                        break;
                    }

                    case(1):
                    {
                        break;
                    }

                    case(2):
                    {
                        break;
                    }
                }
            }


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
        SC_THREAD(run);
        dont_initialize();
        sensitive << clk.pos();

        // trace signals
        sc_trace(tf, rst,   "m_controller.rst");
        sc_trace(tf, clk,   "m_controller.clk");

        sc_trace(tf, layer_id,   "m_controller.layer_id");
        sc_trace(tf, layer_id_type,   "m_controller.layer_id_type");
        sc_trace(tf, layer_id_valid,   "m_controller.layer_id_valid");

        sc_trace(tf, data_received,   "m_controller.data_received");
        sc_trace(tf, data_float,   "m_controller.data_float");
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