#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include "pe.h"
#include <cstring>
#include <helperfunction.h>
#include <deque>
#include "utils.h"
#include "conv_functions.h"
#include "LAYER_PARAM.h"
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
    bool tail_received_f = false;

    Packet *pkt_tx, *pkt_rx;

    // Trace file
    sc_trace_file *tf;

    // global variable
    vector<float> weights_q;
    vector<float> biases_q;
    vector<float> img_q;
    vector<float> result;

    bool done_processing_f = false;
    bool start_sending_f = true;

    void send_packet()
    {
        int flit_counts = 0;
        int num_of_flits;

        std::deque<float> datas_q;

        int src_id;
        int dest_id;
        int data_type;

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
                start_sending_f = false;
            }
            else if (done_processing_f == true)
            {
                cout << "Packetlizing" << endl;
                pkt_tx = new Packet;

                // Setup pkt
                pkt_tx->source_id = id;

                // cout << "Packetlizing src id" << endl;
                dest_id = 0;
                // determine dst id
                switch (id)
                {
                case (1):
                    dest_id = 2;
                    break;
                case (2):
                    dest_id = 3;
                    break;
                case (3):
                    dest_id = 7;
                    break;
                case (7):
                    dest_id = 6;
                    break;
                case (6):
                    dest_id = 5;
                    break;
                case (5):
                    dest_id = 4;
                    break;
                case (4):
                    dest_id = 8;
                    break;
                case (8):
                    dest_id = 0;
                    break;
                }

                pkt_tx->dest_id = dest_id;
                pkt_tx->data_type = 2;
                pkt_tx->datas = result;
                done_processing_f = false;
                start_sending_f = true;
            }
            else if (pkt_tx != nullptr)
            {
                // need checking for null ptr to perform operation on pkt_tx since it is a pointer
                vector<float> datas_f = pkt_tx->datas;
                // copy datas_f into data_q
                for (int i = 0; i < datas_f.size(); i++)
                {
                    datas_q.push_back(datas_f[i]);
                }
                num_of_flits = datas_q.size() + 1; // 1 for additional header
                src_id = pkt_tx->source_id;
                dest_id = pkt_tx->dest_id;
                data_type = pkt_tx->data_type;
                done_sending_flag = false;
                cout << "Core_" << id << " get packet"
                     << "src_id:" << src_id << ", dest_id:" << dest_id << ", num_of_flits:" << num_of_flits << endl;
                pkt_tx = nullptr;

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
                    // data type
                    header.range(23, 22) = data_type;
                    // rest 0
                    header.range(21, 0) = 0;
                    flit_tx.write(header);
                    flit_counts++;
                }
                else if (ack_tx.read() == true && req_tx.read() == true)
                {
                    // cout << "Core_" << id << " sending data" << endl;

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
                // reset
                num_of_flits = 0; // 1 for additional header
                src_id = 0;
                dest_id = 0;
                flit_counts = 0;
                done_sending_flag = false;
                pkt_tx == nullptr;
                start_sending_f = false;
            }

            wait();
        }
    }

    void gather_and_compute()
    {
        for (;;)
        {
            if (tail_received_f == true && pkt_rx != nullptr)
            {
                cout << "Core_id:" << id << " Receiving" << endl;

                int data_type = pkt_rx->data_type;

                switch (data_type)
                {
                case (1): // weights
                {
                    weights_q = pkt_rx->datas;
                    break;
                }
                case (0): // biases
                {
                    biases_q = pkt_rx->datas;
                    break;
                }
                case (2): // imgs
                {
                    img_q = pkt_rx->datas;
                    break;
                }
                default:
                    break;
                }

                // start computing if it is imgs
                if (weights_q.size() > 0 && biases_q.size() > 0 && img_q.size() > 0)
                {
                    // print core id start processing
                    cout << "Core_" << id << " Start processing" << endl;

                    // compute, wrangle the data into the right format
                    float *weights = new float[weights_q.size()];
                    float *biases = new float[biases_q.size()];
                    float *img = new float[img_q.size()];

                    // convert the sc_lv<32> into double for calculation
                    for (int i = 0; i < weights_q.size(); i++)
                    {
                        // float
                        float weights_float;
                        sc_lv<32> weights_sc_lv;

                        weights[i] = weights_q[i];
                    }

                    // convert bias to double
                    for (int i = 0; i < biases_q.size(); i++)
                    {
                        // float
                        float biases_float;
                        sc_lv<32> biases_sc_lv;

                        biases[i] = biases_q[i];
                    }

                    // convert img to double
                    for (int i = 0; i < img_q.size(); i++)
                    {
                        // float
                        float img_float;
                        sc_lv<32> img_sc_lv;

                        img[i] = img_q[i];
                    }

                    if (id == 1)
                        img = assymetrical_padding(img);

                    // Do the calculation
                    switch (id)
                    {

                    // Do Conv,relu,mp
                    case (1):
                    case (2):
                    case (6):
                    {
                        int input_img_channel, input_img_size;
                        int conv_in_channel_num, conv_out_channel_num, conv_kernel_size;
                        int conv_padding, conv_padding_size, conv_stride;

                        int mp_out_channel_num, mp_out_shape;

                        int mp_kernel_size, mp_stride;

                        // Picking the correct parameters for calculations
                        switch (id)
                        {
                        case (1):
                        {
                            // Conversion to correct size for processing
                            input_img_channel = INPUT_IMG_CHANNEL;
                            input_img_size = PAD_IMG_SIZE;

                            // conv
                            conv_out_channel_num = CONV1_OUT_CHANNEL_NUM;
                            conv_in_channel_num = CONV1_IN_CHANNEL_NUM;
                            conv_out_channel_num = CONV1_OUT_CHANNEL_NUM;
                            conv_kernel_size = CONV1_KERNEL_SIZE;

                            conv_stride = CONV1_STRIDE;
                            conv_padding_size = CONV1_PADDING_SIZE;

                            // mp
                            mp_out_channel_num = MP1_OUT_CHANNEL_NUM;
                            mp_out_shape = MP1_OUT_SHAPE;

                            mp_kernel_size = MP1_KERNEL_SIZE;
                            mp_stride = MP1_STRIDE;

                            break;
                        }
                        case (2):
                        {
                            // Conversion to correct size for processing
                            input_img_channel = CONV2_IN_CHANNEL_NUM;
                            input_img_size = MP1_OUT_SHAPE;

                            // conv
                            conv_out_channel_num = CONV2_OUT_CHANNEL_NUM;
                            conv_in_channel_num = CONV2_IN_CHANNEL_NUM;
                            conv_kernel_size = CONV2_KERNEL_SIZE;

                            conv_stride = CONV2_STRIDE;
                            conv_padding_size = CONV2_PADDING_SIZE;

                            // mp
                            mp_out_channel_num = MP2_OUT_CHANNEL_NUM;
                            mp_out_shape = MP2_OUT_SHAPE;

                            mp_kernel_size = MP2_KERNEL_SIZE;
                            mp_stride = MP2_STRIDE;
                            break;
                        }
                        case (6):
                        {
                            break;
                        }
                        }

                        // Data conversion
                        Tensor3d input_tensor = convert1dTo3d(img,
                                                              input_img_channel,
                                                              input_img_size,
                                                              input_img_size);
                        Tensor4d weights_tensor = convert1dTo4d(weights,
                                                                conv_out_channel_num,
                                                                conv_in_channel_num,
                                                                conv_kernel_size,
                                                                conv_kernel_size);
                        Tensor1d bias_tensor = convert1dToTensor1d(biases, conv_out_channel_num);

                        // display img,weights and bias
                        // displayTensor3d(input_tensor, 3, 227, 227, 0, 1, 224, 227, 224, 227);
                        // display bias
                        // display1DTensorVec(bias_tensor,10);

                        // Convolution
                        Tensor3d conv_out = convolution(input_tensor,
                                                        weights_tensor,
                                                        bias_tensor,
                                                        conv_in_channel_num,
                                                        conv_out_channel_num,
                                                        conv_kernel_size,
                                                        conv_stride,
                                                        conv_padding_size); // padding = 0 for layer 1

                        // Display conv1 output
                        // displayTensor3d(conv_out, 1, 16, 16, 0, 1, 0, 16, 0, 16);

                        // RELU
                        conv_out = reluLayer3d(conv_out);

                        // Maxpooling
                        Tensor3d mp_out = maxpooling(conv_out,
                                                     conv_out_channel_num,
                                                     mp_kernel_size,
                                                     mp_stride);

                        // Convert 3d tensor to 1d tensor
                        result = convert3dTo1dVec(mp_out,
                                                  mp_out_channel_num,
                                                  mp_out_shape,
                                                  mp_out_shape);

                        done_processing_f = true;
                        wait();
                        wait();

                        // Display conv1 output after relu
                        displayTensor3d(mp_out, 2, 13, 13, 0, 1, 0, 8, 0, 8);

                        // delete conv1_weights and biases;
                        releaseTensor1dMemory(bias_tensor);
                        releaseTensor4dMemory(weights_tensor);
                    }
                    // Do Conv,relu
                    case (3):
                    case (7):
                    {
                        int input_img_channel, input_img_size;
                        int conv_in_channel_num, conv_out_channel_num, conv_kernel_size;
                        int conv_padding, conv_padding_size, conv_stride;

                        // Parameter selection
                        if (id == 3)
                        {
                            // conv3
                            input_img_channel = CONV3_IN_CHANNEL_NUM;
                            input_img_size = MP2_OUT_SHAPE;
                            conv_out_channel_num = CONV3_OUT_CHANNEL_NUM;
                            conv_kernel_size = CONV3_KERNEL_SIZE;
                            conv_stride = CONV3_STRIDE;
                            conv_padding_size = CONV3_PADDING_SIZE;
                        }
                        else
                        {
                            // conv4
                            input_img_channel = CONV4_IN_CHANNEL_NUM;
                            input_img_size = CONV3_OUT_CHANNEL_NUM;
                            conv_out_channel_num = CONV4_OUT_CHANNEL_NUM;
                            conv_kernel_size = CONV4_KERNEL_SIZE;
                            conv_stride = CONV4_STRIDE;
                            conv_padding_size = CONV4_PADDING_SIZE;
                        }

                        // Data wrangling
                        // Data conversion
                        Tensor3d input_tensor = convert1dTo3d(img,
                                                              input_img_channel,
                                                              input_img_size,
                                                              input_img_size);
                        Tensor4d weights_tensor = convert1dTo4d(weights,
                                                                conv_out_channel_num,
                                                                conv_in_channel_num,
                                                                conv_kernel_size,
                                                                conv_kernel_size);
                        Tensor1d bias_tensor = convert1dToTensor1d(biases, conv_out_channel_num);

                        // Convolution
                        Tensor3d conv_out = convolution(input_tensor,
                                                        weights_tensor,
                                                        bias_tensor,
                                                        conv_in_channel_num,
                                                        conv_out_channel_num,
                                                        conv_kernel_size,
                                                        conv_stride,
                                                        conv_padding_size); // padding = 0 for layer 1

                        // Display conv1 output
                        // displayTensor3d(conv_out, 1, 16, 16, 0, 1, 0, 16, 0, 16);

                        // RELU
                        conv_out = reluLayer3d(conv_out);

                        // Convert3d to 1d float
                        result = convert3dTo1dVec(conv_out,
                                                  conv_out_channel_num,
                                                  input_img_size,
                                                  input_img_size);

                        done_processing_f = true;
                        wait();
                        wait();
                    }

                    // Do FC
                    case (4):
                    case (5):
                    case (8):
                    {
                        int output_channel_num;
                        int input_channel_num;

                        // Parameter selection
                        if (id == 4)
                        {
                            output_channel_num = FC6_OUT_NUM;
                            input_channel_num = FC6_IN_NUM;
                        }
                        else if (id == 5)
                        {
                            output_channel_num = FC7_OUT_NUM;
                            input_channel_num = FC7_IN_NUM;
                        }
                        else
                        {
                            output_channel_num = FC8_OUT_NUM;
                            input_channel_num = FC8_IN_NUM;
                        }

                        // Data conversion
                        Tensor2d weights_tensor = convert1dTo2d(weights, output_channel_num, input_channel_num);
                        Tensor1d bias_tensor = convert1dToTensor1d(biases, output_channel_num);
                        Tensor1d input_tensor = convert1dToTensor1d(img, output_channel_num);

                        // Note if fc8 dont use relu
                        if (id == 8)
                        {
                            result = fc_layer(input_tensor, weights_tensor, bias_tensor, output_channel_num, input_channel_num);
                        }
                        else
                        {
                            result = fc_layer(input_tensor, weights_tensor, bias_tensor, output_channel_num, input_channel_num);
                            result = reluLayer1d(result);
                        }

                        done_processing_f = true;
                        wait();
                        wait();
                    }
                    }
                }
            }

            wait();
        }
    }

    void receive_packet()
    {
        for (;;)
        {
            flit_size_t flit = flit_rx.read();
            tail_received_f = false;

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
                        pkt_rx->data_type = flit.range(23, 22).to_uint();

                        cout << "Core_" << id << " receive header, src_id:" << pkt_rx->source_id << ", dest_id:" << pkt_rx->dest_id << ", data type: " << pkt_rx->data_type << endl;
                    }
                }
                else if (flit.range(33, 32) == 0b01) // tail received
                {
                    // tail
                    float temp = sc_lv_to_float(flit.range(31, 0));
                    if (pkt_rx != nullptr)
                        pkt_rx->datas.push_back(temp);

                    cout << "Core_" << id << " receive tail, data:" << temp << endl;

                    tail_received_f = true;
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

        SC_THREAD(gather_and_compute);
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