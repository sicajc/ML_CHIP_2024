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

SC_MODULE(CONV_RELU_MP2)
{
    // Given 4 inputs
    sc_port<sc_fifo_in_if<double *>> tensor3dImg_i;
    sc_port<sc_fifo_in_if<bool>> valid_i;
    sc_in<bool> clk;

    // Output uses fifo port fort outer connection
    sc_port<sc_fifo_out_if<bool>> valid_o;
    sc_port<sc_fifo_out_if<double *>> tensor3dImg_o;

    void run()
    {
        for (;;)
        {
            // Wait for the next trigger
            wait();
            valid_o->write(0);
            if (valid_i->read() == 1) // Do this only if valid is high
            {
                std::string FILE_DIR = "./data/";
                // vvvvv put your code here
                // cout << "CONV_RELU_MP2 start" << endl;
                // cout << "=========================================" << endl;
                // cout << "Start Reading data" << endl;
                // Read in conv2 weights and biases
                Tensor4d conv2_weights(CONV2_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV2_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV2_KERNEL_SIZE, std::vector<double>(CONV2_KERNEL_SIZE))));
                Tensor1d conv2_bias(CONV2_OUT_CHANNEL_NUM);

                conv2_weights = readAndResizeWeights4d(FILE_DIR+"conv2_weight.txt", CONV2_OUT_CHANNEL_NUM, CONV2_IN_CHANNEL_NUM, CONV2_KERNEL_SIZE, CONV2_KERNEL_SIZE);
                conv2_bias = read1DTensor(FILE_DIR+"conv2_bias.txt", CONV2_OUT_CHANNEL_NUM);

                // convert 1d double array to 3d tensor

                // cout << "Convert 1d to 3d" << endl;

                Tensor3d tensor3dImg_i1 = convert1dTo3d(tensor3dImg_i->read(), MP1_OUT_CHANNEL_NUM,MP1_OUT_SHAPE,MP1_OUT_SHAPE);

                // displayTensor3d(tensor3dImg_i1, MP1_OUT_CHANNEL_NUM,MP1_OUT_SHAPE,MP1_OUT_SHAPE, 0, 2, 0, 10, 0, 10);
                // Conv2
                // cout << "Conv2 start" << endl;
                Tensor3d conv2_out = convolution(tensor3dImg_i1, conv2_weights, conv2_bias, 64, 192, 5, 1, 2);
                // displayTensor3d(conv2_out, 192, 16, 16, 0, 2, 0, 10, 0, 10);

                // relu
                // cout << "Relu2 start" << endl;
                conv2_out = reluLayer3d(conv2_out);

                // Maxpool2
                // cout << "Maxpool2 start" << endl;
                Tensor3d maxpool2_out = maxpooling(conv2_out, MP2_OUT_CHANNEL_NUM, 3, 2);

                // Display conv1 output after MAX pooling
                // displayTensor3d(maxpool2_out, MP2_OUT_CHANNEL_NUM, MP2_OUT_SHAPE, MP2_OUT_SHAPE, 0, 2, 0, 10, 0, 10);

                // Write out the value
                // Convert 3d tensor to 1d tensor
                double *result = convert3dTo1d(maxpool2_out, MP2_OUT_CHANNEL_NUM, MP2_OUT_SHAPE, MP2_OUT_SHAPE);

                wait();
                // Using fifo for connection
                tensor3dImg_o->write(result);
                valid_o->write(1);

                // cout << "===================================CONV_RELU_MP2 ends=========================================" << endl;

                // delete conv2_weights and biases;
                releaseTensor4dMemory(conv2_weights);
                releaseTensor1dMemory(conv2_bias);
            }
        }
    }

    SC_CTOR(CONV_RELU_MP2)
    {
        SC_THREAD(run);
        dont_initialize();
        sensitive << clk.pos();
    }
};