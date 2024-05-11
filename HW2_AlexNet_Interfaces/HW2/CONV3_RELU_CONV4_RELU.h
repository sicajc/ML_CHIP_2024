#include <systemc.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <LAYER_PARAM.h>
#include <utils.h>
#include <conv_functions.h>

typedef std::vector<double> Tensor1d;
typedef std::vector<std::vector<double>> Tensor2d;
typedef std::vector<std::vector<std::vector<double>>> Tensor3d;
typedef std::vector<std::vector<std::vector<std::vector<double>>>> Tensor4d;

SC_MODULE(CONV3_RELU_CONV4_RELU)
{
public:
    // Given 4 inputs
    //change to sc_port fifo
    sc_port<sc_fifo_in_if<double *>> tensor3dImg_i;
    sc_port<sc_fifo_in_if<bool>> valid_i;
    sc_in<bool> clk;

    // Output uses fifo port fort outer connection
    sc_port<sc_fifo_out_if<bool>> valid_o;
    sc_port<sc_fifo_out_if<double *>> tensor3dImg_o;

    // Event for trigger
    sc_event fifo_write_event;

    void run()
    {
        for (;;)
        {
            // Wait for the next trigger
            wait();
            valid_o->write(0);
            if (valid_i->read() == 1) // Do this only if valid is high
            {
                // Conv3
                // cout << "Conv3 start" << endl;

                std::string FILE_DIR = "./data/";

                // convert 1d double array to 3d tensor
                // cout << "Conversion to 3d tensor" << endl;
                Tensor3d tensor3dImg_i1 = convert1dTo3d(tensor3dImg_i->read(), CONV3_IN_CHANNEL_NUM, MP2_OUT_SHAPE, MP2_OUT_SHAPE);

                // Read in conv3 weights and biases
                Tensor4d conv3_weights(CONV3_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV3_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV3_KERNEL_SIZE, std::vector<double>(CONV3_KERNEL_SIZE))));
                Tensor1d conv3_bias(CONV3_OUT_CHANNEL_NUM);

                conv3_weights = readAndResizeWeights4d(FILE_DIR + "conv3_weight.txt", CONV3_OUT_CHANNEL_NUM, CONV3_IN_CHANNEL_NUM, CONV3_KERNEL_SIZE, CONV3_KERNEL_SIZE);
                conv3_bias = read1DTensor(FILE_DIR + "conv3_bias.txt", CONV3_OUT_CHANNEL_NUM);

                Tensor3d conv3_out = convolution(tensor3dImg_i1, conv3_weights, conv3_bias, 192, 384, 3, 1, 1);

                // relu
                // cout << "Relu3 start" << endl;
                conv3_out = reluLayer3d(conv3_out);
                // cout<<conv3_out<<endl;
                // cout<<conv3_out[0]<<endl;

                // Delete conv3 weights and biases
                releaseTensor4dMemory(conv3_weights);
                releaseTensor1dMemory(conv3_bias);

                // Read in conv4 weights and biases
                Tensor4d conv4_weights(CONV4_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV4_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV4_KERNEL_SIZE, std::vector<double>(CONV4_KERNEL_SIZE))));
                Tensor1d conv4_bias(CONV4_OUT_CHANNEL_NUM);

                conv4_weights = readAndResizeWeights4d(FILE_DIR + "conv4_weight.txt", CONV4_OUT_CHANNEL_NUM, CONV4_IN_CHANNEL_NUM, CONV4_KERNEL_SIZE, CONV4_KERNEL_SIZE);
                conv4_bias = read1DTensor(FILE_DIR + "conv4_bias.txt", CONV4_OUT_CHANNEL_NUM);

                // Conv4
                // cout << "Conv4 start" << endl;
                Tensor3d conv4_out = convolution(conv3_out, conv4_weights, conv4_bias, 384, 256, 3, 1, 1);

                // relu
                // cout << "Relu4 start" << endl;
                conv4_out = reluLayer3d(conv4_out);

                // displayTensor3d(conv4_out, 256, 10, 10, 0, 5, 0, 10,0,10);

                // Write out the value
                // convert to 1d
                double *result = convert3dTo1d(conv4_out, CONV4_OUT_CHANNEL_NUM, CONV4_OUT_SHAPE, CONV4_OUT_SHAPE);

                // Using fifo for connection
                wait();
                tensor3dImg_o->write(result);
                valid_o->write(1);

                // Delete conv4 weights and biases
                releaseTensor4dMemory(conv4_weights);
                releaseTensor1dMemory(conv4_bias);

                // cout << "CONV3_CONV4_RELU ENDS"<<endl;
            }
        }
    }

    SC_CTOR(CONV3_RELU_CONV4_RELU)
    {
        SC_THREAD(run);
        dont_initialize();
        sensitive << clk.pos();
    }
};