#include <systemc.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <LAYER_PARAM.h>
#include <utils.h>
#include <conv_functions.h>
#include <CONV_RELU_MP1.h>
#include <CONV_RELU_MP2.h>
#include <CONV3_RELU_CONV4_RELU.h>
#include <CONV_RELU_MP5.h>
#include <FC.h>

typedef std::vector<double> Tensor1d;
typedef std::vector<std::vector<double>> Tensor2d;
typedef std::vector<std::vector<std::vector<double>>> Tensor3d;
typedef std::vector<std::vector<std::vector<std::vector<double>>>> Tensor4d;

SC_MODULE(ALEXNET)
{
    // Given 4 inputs
    sc_in<double *> tensor3dImgi;
    sc_in<bool> clk, rst, start;
    sc_out<double *> fully_connected_result;
    sc_out<bool> valid_o;

    sc_signal<double *> padding_img_o;
    sc_signal<double *> conv1_out_o;
    sc_signal<double *> conv2_out_o;
    sc_signal<double *> conv3_out_o;
    sc_signal<double *> conv4_out_o;
    sc_signal<double *> conv5_out_o;
    sc_signal<double *> fc6_out_o;
    sc_signal<double *> fc7_out_o;
    sc_signal<double *> fc8_out_o;

    sc_signal<bool> valid_conv1_o;
    sc_signal<bool> valid_conv2_o;
    sc_signal<bool> valid_conv3_o;
    sc_signal<bool> valid_conv4_o;
    sc_signal<bool> valid_conv5_o;
    sc_signal<bool> valid_fc6_o;
    sc_signal<bool> valid_fc7_o;
    sc_signal<bool> valid_fc8_o;
    sc_signal<bool> padded_img_valid_o;

    // Channels
    // Declare fifos here, the size is 16 here
    sc_fifo<double *> img_pad_fifo;
    sc_fifo<double *> pad_conv1_fifo;
    sc_fifo<double *> conv1conv2_fifo;
    sc_fifo<double *> conv2_conv34_fifo;
    sc_fifo<double *> conv34_conv5_fifo;
    sc_fifo<double *> conv4conv5_fifo;
    sc_fifo<double *> conv5fc6_fifo;
    sc_fifo<double *> fc6fc7_fifo;
    sc_fifo<double *> fc7fc8_fifo;
    sc_fifo<double *> fc8_out_fifo;

    sc_event conv1conv2_event;
    sc_event conv2conv3_event;
    sc_event conv3conv4_event;
    sc_event conv4conv5_event;
    sc_event conv5fc6_event;
    sc_event fc6fc7_event;
    sc_event fc7fc8_event;

    // valid fifo
    sc_fifo<bool> start_fifo;
    sc_fifo<bool> pad_conv1_valid_fifo;
    sc_fifo<bool> conv1conv2_valid_fifo;
    sc_fifo<bool> conv2_conv34_valid_fifo;
    sc_fifo<bool> conv34_conv5_valid_fifo;
    sc_fifo<bool> conv4conv5_valid_fifo;
    sc_fifo<bool> conv5fc6_valid_fifo;
    sc_fifo<bool> fc6fc7_valid_fifo;
    sc_fifo<bool> fc7fc8_valid_fifo;
    sc_fifo<bool> fc8_out_valid_fifo;

    // Module declartion
    CONV_RELU_MP1 m_conv_relu_mp1;
    CONV_RELU_MP2 m_conv_relu_mp2;
    CONV3_RELU_CONV4_RELU m_conv3_relu_conv4_relu;
    CONV_RELU_MP5 m_conv_relu_mp5;
    FC m_fc6{"m_fc6"};
    FC m_fc7{"m_fc7"};
    FC m_fc8{"m_fc8"};

    void input_img()
    {
        for (;;)
        {
            wait();
            if (start.read() == 1)
            {
                start_fifo.write(1);
                img_pad_fifo.write(tensor3dImgi.read());
            }
            else
            {
                start_fifo.write(0);
                img_pad_fifo.write(tensor3dImgi.read());
            }
        }
    }

    void assymetric_zero_padding()
    {
        for (;;)
        {
            wait();

            if (start_fifo.read() == 1)
            {
                // make this color print out blue on linux terminal
                // cout << "\033[1;34m";
                // cout << "===========================================================================" << endl;
                // cout << "                               Alexnet start                               " << endl;
                // cout << "===========================================================================" << endl;
                // // make this color purple on linux
                // cout << "\033[1;35m";
                // cout << "                   Input tensor is of the following                         " << endl;
                // display1DTensor(tensor3dImgi.read(), 10);
                // // make this color print out default on linux terminal
                // cout << "\033[0m";

                // display tensor3dImgi


                // convert tensor3dImgi to 3dtensor type
                Tensor3d tensor3dImg_i1 = convert1dTo3d(tensor3dImgi.read(), INPUT_IMG_CHANNEL, INPUT_IMG_WIDTH, INPUT_IMG_HEIGHT);
                // cout << "Conversion to 3d tensor" << endl;

                Tensor3d cat_img_padded(INPUT_IMG_CHANNEL, std::vector<std::vector<double>>(INPUT_IMG_WIDTH + 3, std::vector<double>(INPUT_IMG_HEIGHT + 3)));

                for (int c = 0; c < INPUT_IMG_CHANNEL; ++c)
                {
                    for (int w = 0; w < INPUT_IMG_WIDTH; ++w)
                    {
                        for (int h = 0; h < INPUT_IMG_HEIGHT; ++h)
                        {
                            cat_img_padded[c][w + 2][h + 2] = tensor3dImg_i1[c][w][h];
                        }
                    }
                }

                // convert 3d tensor to 1d tensor


                double* padded_img= convert3dTo1d(cat_img_padded, INPUT_IMG_CHANNEL, INPUT_IMG_WIDTH + 3, INPUT_IMG_HEIGHT + 3);
                // cout << "Conversion to 1d tensor" << endl;

                // print out padded img
                // cout << "===============================Padded image=======================================" << endl;
                // display it in 3d
                // displayTensor3d(cat_img_padded, 3, INPUT_IMG_CHANNEL+3, INPUT_IMG_WIDTH + 3, 0, 1, 0, 5, 0, 5);

                pad_conv1_fifo.write(padded_img);
                pad_conv1_valid_fifo.write(1);

                // release memory of cat_img_padded
                releaseTensor3dMemory(cat_img_padded);
                //release memory of tensor img 3d
                releaseTensor3dMemory(tensor3dImg_i1);
                //release pad img
                // delete[] padded_img;
            }
            else
            {
                pad_conv1_valid_fifo.write(0);
            }
        }
    }
    // output connection, returning the softmax value
    void output_connection()
    {
        for (;;)
        {
            wait();
            if (fc8_out_valid_fifo.read() == 1)
            {
                std::string file_dir = "./data/";
                // do softmax
                double *fc8_out = fc8_out_fifo.read();

                // convertTensor1dTo1d
                Tensor1d softmax_out = softmax(convert1dToTensor1d(fc8_out, 1000));

                // output the softmax value and out valid
                fully_connected_result.write(convertTensor1dTo1d(softmax_out, 1000));
                valid_o.write(1);

                // show softmax output
                // display1DTensor(softmax_out, 10);

                // read in classes
                std::vector<std::string> classes = readClasses(file_dir + "imagenet_classes.txt");

                // map softmax to class
                // std::string class_name = mapSoftmaxToClass(softmax_out, classes);

                // printSoftmaxValues(softmax_out, classes);

                // getTopKClasses(const Tensor1d& softmax, const std::vector<std::string>& classes, int k)
                std::vector<std::pair<std::string, double>> top5_classes = sortClassesBasedOnSoftmax(softmax_out, classes);

                // Print out dash
                // Print classified results

                //print out a box for this classification results on linux terminal
                // print out yellow color
                cout << "\033[1;33m";
                cout << "====================Classification   Results===============================" << endl;

                // print top 5 classes and its correspondent softmax
                for (size_t i = 0; i < 5; ++i)
                {
                    std::cout << top5_classes[i].first << ": " << top5_classes[i].second * 100 << std::endl;
                }

                cout << "======================Alexnet ends=========================================" << endl;
                //change back to default color
                cout << "\033[0m";
            }
            else
            {
                valid_o.write(0);
            }
        }
    }

    // check fifo
    void check_fifo()
    {
        for (;;)
        {
            wait(m_conv_relu_mp1.fifo_write_event);
            cout << "conv1conv2_fifo: " << conv1conv2_fifo.num_available() << endl;
            // display the top element of fifo
        }
    }

    SC_CTOR(ALEXNET) : m_conv_relu_mp1("m_conv_relu_mp1"),
                       m_conv_relu_mp2("m_conv_relu_mp2"),
                       m_conv3_relu_conv4_relu("m_conv3_relu_conv4_relu"),
                       m_conv_relu_mp5("m_conv_relu_mp5"),
                       m_fc6("m_fc6"),
                       m_fc7("m_fc7")
    {
        m_conv_relu_mp1.clk(clk);
        m_conv_relu_mp1.tensor3dImg_i(pad_conv1_fifo);
        m_conv_relu_mp1.valid_i(pad_conv1_valid_fifo);

        m_conv_relu_mp1.tensor3dImg_o(conv1conv2_fifo);
        m_conv_relu_mp1.valid_o(conv1conv2_valid_fifo);

        m_conv_relu_mp2.clk(clk);
        m_conv_relu_mp2.tensor3dImg_i(conv1conv2_fifo);
        m_conv_relu_mp2.valid_i(conv1conv2_valid_fifo);

        m_conv_relu_mp2.tensor3dImg_o(conv2_conv34_fifo);
        m_conv_relu_mp2.valid_o(conv2_conv34_valid_fifo);

        // connecting conv3 with conv2
        m_conv3_relu_conv4_relu.clk(clk);
        m_conv3_relu_conv4_relu.tensor3dImg_i(conv2_conv34_fifo);
        m_conv3_relu_conv4_relu.valid_i(conv2_conv34_valid_fifo);

        m_conv3_relu_conv4_relu.tensor3dImg_o(conv34_conv5_fifo);
        m_conv3_relu_conv4_relu.valid_o(conv34_conv5_valid_fifo);

        // connect conv34 with conv5
        m_conv_relu_mp5.clk(clk);
        m_conv_relu_mp5.tensor3dImg_i(conv34_conv5_fifo);
        m_conv_relu_mp5.valid_i(conv34_conv5_valid_fifo);

        m_conv_relu_mp5.tensor3dImg_o(conv5fc6_fifo);
        m_conv_relu_mp5.valid_o(conv5fc6_valid_fifo);

        // connect conv5 with fc6
        m_fc6.file_dir = "./data/";
        m_fc6.weight_dir = "fc6_weight.txt";
        m_fc6.bias_dir = "fc6_bias.txt";
        m_fc6.fc_num = 6;
        m_fc6.input_channel_num = 9216;
        m_fc6.output_channel_num = 4096;

        m_fc6.clk(clk);
        m_fc6.tensor1dImg_i(conv5fc6_fifo);
        m_fc6.valid_i(conv5fc6_valid_fifo);

        m_fc6.tensor1dImg_o(fc6fc7_fifo);
        m_fc6.valid_o(fc6fc7_valid_fifo);

        // connect fc6 with fc7
        m_fc7.file_dir = "./data/";
        m_fc7.weight_dir = "fc7_weight.txt";
        m_fc7.bias_dir = "fc7_bias.txt";
        m_fc7.fc_num = 7;
        m_fc7.input_channel_num = 4096;
        m_fc7.output_channel_num = 4096;

        m_fc7.clk(clk);
        m_fc7.tensor1dImg_i(fc6fc7_fifo);
        m_fc7.valid_i(fc6fc7_valid_fifo);

        m_fc7.tensor1dImg_o(fc7fc8_fifo);
        m_fc7.valid_o(fc7fc8_valid_fifo);

        // connect fc7 to fc8
        m_fc8.file_dir = "./data/";
        m_fc8.weight_dir = "fc8_weight.txt";
        m_fc8.bias_dir = "fc8_bias.txt";
        m_fc8.fc_num = 8;
        m_fc8.input_channel_num = 4096;

        m_fc8.clk(clk);
        m_fc8.tensor1dImg_i(fc7fc8_fifo);
        m_fc8.valid_i(fc7fc8_valid_fifo);

        m_fc8.tensor1dImg_o(fc8_out_fifo);
        m_fc8.valid_o(fc8_out_valid_fifo);

        // input img
        SC_THREAD(input_img);
        dont_initialize();
        sensitive << clk.pos(); // Only synchrnoize it with the global clock!

        // output connection
        SC_THREAD(output_connection);
        dont_initialize();
        sensitive << clk.pos(); // Only synchrnoize it with the global clock!

        // Thread declarations
        SC_THREAD(assymetric_zero_padding);
        dont_initialize();
        sensitive << clk.pos(); // Only synchrnoize it with the global clock!

        SC_THREAD(check_fifo);
    }
};