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

SC_MODULE(ALEXNET)
{
    sc_in<sc_uint<4>> in1, in2;
    sc_out<sc_uint<9>> out; // Output is a vector of 1000 inputs

    sc_signal<sc_uint<8>> temp;

    void process()
    {
        std::string file_dir = "./data/";
        std::string file_name = "dog.txt";
        std::string file_path = file_dir + file_name;

        // vvvvv put your code here
        cout << "Alexnet start" << endl;
        cout << "=========================================" << endl;
        cout << "Start Reading data" << endl;

        // Read in cat img
        Tensor3d cat_img(INPUT_IMG_CHANNEL, std::vector<std::vector<double>>(INPUT_IMG_WIDTH, std::vector<double>(INPUT_IMG_HEIGHT)));

        cat_img = readAndResizeImageData3d(file_path, INPUT_IMG_CHANNEL, INPUT_IMG_WIDTH, INPUT_IMG_HEIGHT);

        // Display resized data
        //void displayTensor3d(const Tensor3d& resized_data, int img_channel, int img_width, int img_height, int start_channel, int end_channel, int start_width, int end_width, int start_height, int end_height
        // displayTensor3d(cat_img, 1, 16, 16, 0, 1, 0, 16, 0, 16);

        // Read in conv1 weights and bias
        Tensor4d conv1_weights(CONV1_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV1_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV1_KERNEL_SIZE, std::vector<double>(CONV1_KERNEL_SIZE))));
        Tensor1d conv1_bias(CONV1_OUT_CHANNEL_NUM);

        conv1_weights = readAndResizeWeights4d(file_dir+"conv1_weight.txt", CONV1_OUT_CHANNEL_NUM, CONV1_IN_CHANNEL_NUM, CONV1_KERNEL_SIZE, CONV1_KERNEL_SIZE);
        conv1_bias = read1DTensor(file_dir + "conv1_bias.txt", CONV1_OUT_CHANNEL_NUM);

        // Display resized data

        // displayTensor4d(conv1_weights, 1, 3, 5, 5);
        // display1DTensor(conv1_bias,20);

        // Do zero-padding on the img make it 3x227x227
        Tensor3d cat_img_padded(INPUT_IMG_CHANNEL, std::vector<std::vector<double>>(INPUT_IMG_WIDTH + 3, std::vector<double>(INPUT_IMG_HEIGHT + 3)));

        for (int c = 0; c < INPUT_IMG_CHANNEL; ++c)
        {
            for (int w = 0; w < INPUT_IMG_WIDTH; ++w)
            {
                for (int h = 0; h < INPUT_IMG_HEIGHT; ++h)
                {
                    cat_img_padded[c][w + 2][h + 2] = cat_img[c][w][h];
                }
            }
        }

        // displayTensor3d(cat_img_padded, 1, 227, 227, 224, 224, 227, 227);

        // Conv1
        cout << "Conv1 start" << endl;
        Tensor3d conv1_out = convolution(cat_img_padded,conv1_weights,conv1_bias,3,64,11,4,0); // padding = 0

        // Display conv1 output
        // displayTensor3d(conv1_out, 1, 16, 16, 0, 1, 0, 16, 0, 16);
        // Relu
        cout << "Relu start" << endl;

        conv1_out = reluLayer3d(conv1_out);

        // (64,3,2)
        // Maxpool 2d
        cout << "Maxpool1 start" << endl;
        Tensor3d maxpool1_out = maxpooling(conv1_out,64,3,2);

        // Display conv1 output after relu
        // displayTensor3d(maxpool1_out, 1, 16, 16, 0, 1, 0, 16, 0, 16);


        // delete conv1_weights and biases;
        releaseTensor4dMemory(conv1_weights);
        releaseTensor1dMemory(conv1_bias);

        // Read in conv2 weights and biases
        Tensor4d conv2_weights(CONV2_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV2_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV2_KERNEL_SIZE, std::vector<double>(CONV2_KERNEL_SIZE))));
        Tensor1d conv2_bias(CONV2_OUT_CHANNEL_NUM);

        conv2_weights = readAndResizeWeights4d(file_dir+"conv2_weight.txt", CONV2_OUT_CHANNEL_NUM, CONV2_IN_CHANNEL_NUM, CONV2_KERNEL_SIZE, CONV2_KERNEL_SIZE);
        conv2_bias = read1DTensor(file_dir+"conv2_bias.txt", CONV2_OUT_CHANNEL_NUM);

        //Conv2
        cout << "Conv2 start" << endl;
        Tensor3d conv2_out = convolution(maxpool1_out,conv2_weights,conv2_bias,64,192,5,1,2);

        // relu
        cout << "Relu2 start" << endl;
        conv2_out = reluLayer3d(conv2_out);

        // Maxpool2
        cout << "Maxpool2 start" << endl;
        Tensor3d maxpool2_out = maxpooling(conv2_out,192,3,2);

        // Display conv2 output after maxpooling
        // displayTensor3d(maxpool2_out, 1, 10, 10, 0, 1, 0, 5, 0, 5);

        // delete conv2_weights and biases;
        releaseTensor4dMemory(conv2_weights);
        releaseTensor1dMemory(conv2_bias);


        // Read in conv3 weights and biases
        Tensor4d conv3_weights(CONV3_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV3_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV3_KERNEL_SIZE, std::vector<double>(CONV3_KERNEL_SIZE))));
        Tensor1d conv3_bias(CONV3_OUT_CHANNEL_NUM);

        conv3_weights = readAndResizeWeights4d(file_dir+"conv3_weight.txt", CONV3_OUT_CHANNEL_NUM, CONV3_IN_CHANNEL_NUM, CONV3_KERNEL_SIZE, CONV3_KERNEL_SIZE);
        conv3_bias = read1DTensor(file_dir+"conv3_bias.txt", CONV3_OUT_CHANNEL_NUM);

        // displayTensor4d(conv3_out, 0, 10, 10, 1, 1, 0, 10,0,10);

        //Conv3
        cout << "Conv3 start" << endl;
        Tensor3d conv3_out = convolution(maxpool2_out,conv3_weights,conv3_bias,192,384,3,1,1);

        // relu
        cout << "Relu3 start" << endl;
        conv3_out = reluLayer3d(conv3_out);
        // cout<<conv3_out<<endl;
        // cout<<conv3_out[0]<<endl;

        // displayTensor3d(conv3_out, 6, 10, 10, 0, 1, 0, 10,0,10);
        // /*
        // Delete conv3 weights and biases
        releaseTensor4dMemory(conv3_weights);
        releaseTensor1dMemory(conv3_bias);

        // Read in conv4 weights and biases
        Tensor4d conv4_weights(CONV4_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV4_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV4_KERNEL_SIZE, std::vector<double>(CONV4_KERNEL_SIZE))));
        Tensor1d conv4_bias(CONV4_OUT_CHANNEL_NUM);

        conv4_weights = readAndResizeWeights4d(file_dir+"conv4_weight.txt", CONV4_OUT_CHANNEL_NUM, CONV4_IN_CHANNEL_NUM, CONV4_KERNEL_SIZE, CONV4_KERNEL_SIZE);
        conv4_bias = read1DTensor(file_dir+"conv4_bias.txt", CONV4_OUT_CHANNEL_NUM);

        // Conv4
        cout << "Conv4 start" << endl;
        Tensor3d conv4_out = convolution(conv3_out,conv4_weights,conv4_bias,384,256,3,1,1);

        // relu
        cout << "Relu4 start" << endl;
        conv4_out = reluLayer3d(conv4_out);

        // displayTensor3d(conv4_out, 1, 10, 10, 1, 1, 0, 10,0,10);

        // Delete conv4 weights and biases
        releaseTensor4dMemory(conv4_weights);
        releaseTensor1dMemory(conv4_bias);

        // /*
        // Read in conv5 weights and biases
        Tensor4d conv5_weights(CONV5_OUT_CHANNEL_NUM, std::vector<std::vector<std::vector<double>>>(CONV5_IN_CHANNEL_NUM, std::vector<std::vector<double>>(CONV5_KERNEL_SIZE, std::vector<double>(CONV5_KERNEL_SIZE))));
        Tensor1d conv5_bias(CONV5_OUT_CHANNEL_NUM);

        conv5_weights = readAndResizeWeights4d(file_dir+"conv5_weight.txt", CONV5_OUT_CHANNEL_NUM, CONV5_IN_CHANNEL_NUM, CONV5_KERNEL_SIZE, CONV5_KERNEL_SIZE);
        conv5_bias = read1DTensor(file_dir+"conv5_bias.txt", CONV5_OUT_CHANNEL_NUM);

        // Conv5
        cout << "Conv5 start" << endl;
        Tensor3d conv5_out = convolution(conv4_out,conv5_weights,conv5_bias,256,256,3,1,1);

        // relu
        cout << "Relu5 start" << endl;

        conv5_out = reluLayer3d(conv5_out);

        // Maxpool5
        cout << "Maxpool5 start" << endl;
        Tensor3d maxpool5_out = maxpooling(conv5_out,256,3,2);

        //void displayTensor3d(const Tensor3d& resized_data, int img_channel, int img_width, int img_height, int start_channel, int end_channel, int start_width, int end_width, int start_height, int end_height)
        // Display conv5 output after maxpooling
        // displayTensor3d(maxpool5_out, 5, 5, 5, 0, 2, 0, 5,0,5);


        cout<<"maxpool5 ends"<<endl;

        // delete conv5 weights and biases;
        releaseTensor4dMemory(conv5_weights);
        releaseTensor1dMemory(conv5_bias);

        // Read in fc6 weights and biases
        Tensor2d fc6_weights(FC6_OUT_NUM, std::vector<double>(FC6_IN_NUM));
        Tensor1d fc6_bias(FC6_OUT_NUM);

        fc6_weights = readAndResizeWeights2d(file_dir+"fc6_weight.txt", FC6_OUT_NUM, FC6_IN_NUM);
        fc6_bias = read1DTensor(file_dir+"fc6_bias.txt", FC6_OUT_NUM);

        //Flatten maxpooling output to 1d
        Tensor1d fc6_in = flattenTensor(maxpool5_out);

        // FC6
        cout << "FC6 start" << endl;
        Tensor1d fc6_out = fc_layer(fc6_in,fc6_weights,fc6_bias,9216,4096);

        // show fc6 output
        // display1DTensor(fc6_out, 10);

        //relu
        cout << "Relu6 start" << endl;
        fc6_out = reluLayer1d(fc6_out);

        // show relu6 output
        // display1DTensor(fc6_out, 10);

        // delete fc6 weights and biases;
        releaseTensor2dMemory(fc6_weights);
        releaseTensor1dMemory(fc6_bias);

        // Read in fc7 weights and biases
        Tensor2d fc7_weights(FC7_OUT_NUM, std::vector<double>(FC7_IN_NUM));
        Tensor1d fc7_bias(FC7_OUT_NUM);

        fc7_weights = readAndResizeWeights2d(file_dir+"fc7_weight.txt", FC7_OUT_NUM, FC7_IN_NUM);
        fc7_bias = read1DTensor(file_dir+"fc7_bias.txt", FC7_OUT_NUM);

        // FC7
        cout << "FC7 start" << endl;
        Tensor1d fc7_out = fc_layer(fc6_out,fc7_weights,fc7_bias,4096,4096);

        // show fc7 output
        // display1DTensor(fc7_out, 10);

        // relu
        cout << "Relu7 start" << endl;
        fc7_out = reluLayer1d(fc7_out);

        //diplay relu7 output
        // display1DTensor(fc7_out, 10);

        // delete fc7 weights and biases;
        releaseTensor2dMemory(fc7_weights);
        releaseTensor1dMemory(fc7_bias);

        // Read in fc8 weights and biases
        Tensor2d fc8_weights(FC8_OUT_NUM, std::vector<double>(FC8_IN_NUM));
        Tensor1d fc8_bias(FC8_OUT_NUM);

        fc8_weights = readAndResizeWeights2d(file_dir+"fc8_weight.txt", FC8_OUT_NUM, FC8_IN_NUM);
        fc8_bias = read1DTensor(file_dir+"fc8_bias.txt", FC8_OUT_NUM);

        // FC8
        cout << "FC8 start" << endl;
        Tensor1d fc8_out = fc_layer(fc7_out,fc8_weights,fc8_bias,4096,1000);

        // show fc8 output
        // display1DTensor(fc8_out, 10);

        // delete fc8 weights and biases;
        releaseTensor2dMemory(fc8_weights);
        releaseTensor1dMemory(fc8_bias);

        // softmax
        cout << "Softmax start" << endl;
        Tensor1d softmax_out = softmax(fc8_out);

        // show softmax output
        // display1DTensor(softmax_out, 10);

        // read in classes
        std::vector<std::string> classes = readClasses(file_dir+"imagenet_classes.txt");

        // map softmax to class
        // std::string class_name = mapSoftmaxToClass(softmax_out, classes);

        // printSoftmaxValues(softmax_out, classes);

        // getTopKClasses(const Tensor1d& softmax, const std::vector<std::string>& classes, int k)
        std::vector<std::pair<std::string, double>> top5_classes = sortClassesBasedOnSoftmax(softmax_out, classes);

        // Print out dash
        // Print classified results

        cout << "===============Classification   Results===============================" << endl;


        // print top 5 classes and its correspondent softmax
        for (size_t i = 0; i < 5; ++i)
        {
            std::cout << top5_classes[i].first << ": " << top5_classes[i].second*100 << std::endl;
        }


        cout << "=================Alexnet ends=========================================" << endl;

    }

    SC_CTOR(ALEXNET)
    {
        SC_METHOD(process);
        sensitive << in1 << in2;
    }
};