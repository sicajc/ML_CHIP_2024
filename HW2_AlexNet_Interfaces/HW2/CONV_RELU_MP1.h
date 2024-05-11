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

SC_MODULE(CONV_RELU_MP1)
{
public:
    // Given 4 inputs
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
                std::string FILE_DIR = "./data/";
                // vvvvv put your code here
                // cout << "CONV_RELU_MP1 start" << endl;
                // cout << "=========================================" << endl;
                // cout << "Start Reading data" << endl;
                // Read in conv1 weights and bias
                Tensor4d conv1_weights(CONV1_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV1_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV1_KERNEL_SIZE, std::vector<double>(CONV1_KERNEL_SIZE))));
                Tensor1d conv1_bias(CONV1_OUT_CHANNEL_NUM);

                conv1_weights = readAndResizeWeights4d(FILE_DIR + "conv1_weight.txt", CONV1_OUT_CHANNEL_NUM, CONV1_IN_CHANNEL_NUM, CONV1_KERNEL_SIZE, CONV1_KERNEL_SIZE);
                conv1_bias = read1DTensor(FILE_DIR + "conv1_bias.txt", CONV1_OUT_CHANNEL_NUM);

                // convert 1d double array to 3d tensor
                Tensor3d tensor3dImg_i1 = convert1dTo3d(tensor3dImg_i->read(), INPUT_IMG_CHANNEL, PAD_IMG_SIZE, PAD_IMG_SIZE);

                // display 3d img
                //  displayTensor3d(tensor3dImg_i1, 1, 16, 16, 0, 1, 0, 4, 0, 4);

                // Conv1
                // cout << "Conv1 start" << endl;
                Tensor3d conv1_out = convolution(tensor3dImg_i1, conv1_weights, conv1_bias, 3, 64, 11, 4, 0); // padding = 0

                // Display conv1 output
                // displayTensor3d(conv1_out, 1, 16, 16, 0, 1, 0, 16, 0, 16);
                // Relu
                // cout << "Relu start" << endl;

                conv1_out = reluLayer3d(conv1_out);

                // (64,3,2)
                // Maxpool 2d
                // cout << "Maxpool1 start" << endl;
                Tensor3d maxpool1_out = maxpooling(conv1_out, 64, 3, 2);

                // Display conv1 output after relu
                // displayTensor3d(maxpool1_out, 1, 16, 16, 0, 1, 0, 16, 0, 16);

                // Write out the value
                // Convert 3d tensor to 1d tensor
                double *result = convert3dTo1d(maxpool1_out, MP1_OUT_CHANNEL_NUM, MP1_OUT_SHAPE, MP1_OUT_SHAPE);

                // Simulate cycle of writing in fifo
                wait();
                // Using fifo for connection
                tensor3dImg_o->write(result);
                valid_o->write(1);

                // cout << "===================================CONV_RELU_MP1 ends=========================================" << endl;

                // delete conv1_weights and biases;
                releaseTensor4dMemory(conv1_weights);
                releaseTensor1dMemory(conv1_bias);
            }
        }
    }

    SC_CTOR(CONV_RELU_MP1)
    {
        SC_THREAD(run);
        dont_initialize();
        sensitive << clk.pos();
    }
};