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

SC_MODULE(FC)
{
public:
    // Given 4 inputs
    sc_port<sc_fifo_in_if<double *>> tensor1dImg_i;
    sc_port<sc_fifo_in_if<bool>> valid_i;
    sc_in<bool> clk;

    // Output uses fifo port fort outer connection
    sc_port<sc_fifo_out_if<bool>> valid_o;
    sc_port<sc_fifo_out_if<double *>> tensor1dImg_o;

    std::string file_dir;
    std::string weight_dir;
    std::string bias_dir;
    int fc_num;
    int input_channel_num;
    int output_channel_num;

    void run()
    {
        for (;;)
        {
            // Wait for the next trigger
            wait();
            valid_o->write(0);
            if (valid_i->read() == 1) // Do this only if valid is high
            {
                // std::string file_dir = "./data/";
                // Read in fc6 weights and biases
                Tensor2d fc6_weights(output_channel_num, std::vector<double>(input_channel_num));
                Tensor1d fc6_bias(output_channel_num);

                fc6_weights = readAndResizeWeights2d(file_dir + weight_dir, output_channel_num, input_channel_num);
                fc6_bias = read1DTensor(file_dir + bias_dir, output_channel_num);

                // Flatten maxpooling output to 1d
                // Tensor1d fc6_in = flattenTensor(maxpool5_out);
                // convert double array to tensor1d

                // FC
                // cout << "FC start" << endl;
                Tensor1d fc6_out = fc_layer(convert1dToTensor1d(tensor1dImg_i->read(), input_channel_num),
                                            fc6_weights,
                                            fc6_bias,
                                            input_channel_num,
                                            output_channel_num);

                // show fc6 output
                // display1DTensor(fc6_out, 10);

                // relu
                // cout << "Relu6 start" << endl;
                // check str fc8, if it is fc8, dont use relu

                if(fc_num != 8)
                    fc6_out = reluLayer1d(fc6_out);

                wait(clk->posedge_event());
                // convert tensor1d to double array
                tensor1dImg_o->write(convertTensor1dTo1d(fc6_out, output_channel_num));
                valid_o->write(1);

                // show relu6 output
                // display1DTensorVec(fc6_out, 10);

                // delete fc6 weights and biases;
                releaseTensor2dMemory(fc6_weights);
                releaseTensor1dMemory(fc6_bias);
                // cout << "Fully connected done" << endl;
            }
        }
    }

    SC_CTOR(FC) :  file_dir("./data/"),
                   weight_dir("fc6_weight.txt"),
                   bias_dir("fc6_bias.txt"),
                   input_channel_num(9216),
                   output_channel_num(4096),
                   fc_num(6)
    {
        SC_THREAD(run);
        dont_initialize();
        sensitive << clk.pos();
    }
};