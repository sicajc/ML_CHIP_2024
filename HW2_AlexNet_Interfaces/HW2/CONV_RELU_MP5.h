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

SC_MODULE(CONV_RELU_MP5)
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
                // cout << "CONV_RELU_MP5 start" << endl;
                // cout << "=========================================" << endl;
                // cout << "Start Reading data" << endl;
                // Read in conv5 weights and biases
                Tensor4d conv5_weights(CONV5_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV5_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV5_KERNEL_SIZE, std::vector<double>(CONV5_KERNEL_SIZE))));
                Tensor1d conv5_bias(CONV5_OUT_CHANNEL_NUM);

                conv5_weights = readAndResizeWeights4d(FILE_DIR+"conv5_weight.txt", CONV5_OUT_CHANNEL_NUM, CONV5_IN_CHANNEL_NUM, CONV5_KERNEL_SIZE, CONV5_KERNEL_SIZE);
                conv5_bias = read1DTensor(FILE_DIR+"conv5_bias.txt", CONV5_OUT_CHANNEL_NUM);

                // convert 1d double array to 3d tensor

                // cout << "Convert 1d to 3d" << endl;

                Tensor3d tensor3dImg_i1 = convert1dTo3d(tensor3dImg_i->read(), CONV5_OUT_CHANNEL_NUM,CONV5_OUT_SHAPE,CONV5_OUT_SHAPE);

                // displayTensor3d(tensor3dImg_i1, MP4_OUT_CHANNEL_NUM,MP4_OUT_SHAPE,MP4_OUT_SHAPE, 0, 2, 0, 10, 0, 10);
                // Conv5
                // cout << "Conv5 start" << endl;
                Tensor3d conv5_out = convolution(tensor3dImg_i1, conv5_weights, conv5_bias, 256, 256, 3, 1, 1);
                // displayTensor3d(conv5_out, 256, 8, 8, 0, 2, 0, 10, 0, 10);

                // relu
                // cout << "Relu5 start" << endl;
                conv5_out = reluLayer3d(conv5_out);

                // Maxpool5
                // cout << "Maxpool5 start" << endl;
                Tensor3d maxpool5_out = maxpooling(conv5_out, MP5_OUT_CHANNEL_NUM, 3, 2);

                // Display conv5 output after MAX pooling
                // displayTensor3d(maxpool5_out, MP5_OUT_CHANNEL_NUM, MP5_OUT_SHAPE, MP5_OUT_SHAPE, 0, 2, 0, 10, 0, 10);

                // Write out the value
                // Convert 3d tensor to 1d tensor
                double *result = convert3dTo1d(maxpool5_out, MP5_OUT_CHANNEL_NUM, MP5_OUT_SHAPE, MP5_OUT_SHAPE);

                wait(clk->posedge_event());
                // Using fifo for connection
                tensor3dImg_o->write(result);
                valid_o->write(1);

                // cout << "===================================CONV_RELU_MP5 ends=========================================" << endl;

                // delete conv5_weights and biases;
                releaseTensor4dMemory(conv5_weights);
                releaseTensor1dMemory(conv5_bias);
            }
        }
    }

    SC_CTOR(CONV_RELU_MP5)
    {
        SC_THREAD(run);
        dont_initialize();
        sensitive << clk.pos();
    }
};