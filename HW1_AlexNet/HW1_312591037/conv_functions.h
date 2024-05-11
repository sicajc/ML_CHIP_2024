#include <vector>
typedef std::vector<double> Tensor1d;
typedef std::vector<std::vector<std::vector<double>>> Tensor3d;
typedef std::vector<std::vector<std::vector<std::vector<double>>>> Tensor4d;

Tensor3d convolution(const Tensor3d& input, const Tensor4d& weights, const std::vector<double>& biases,
                     int in_channel_num, int out_channel_num, int kernel_size, int stride, int padding) {
    int in_height = input[0].size();
    int in_width = input[0][0].size();
    int out_height = (in_height + 2 * padding - kernel_size) / stride + 1;
    int out_width = (in_width + 2 * padding - kernel_size) / stride + 1;

    // Do zero padding to input
    Tensor3d padded_input(in_channel_num, std::vector<std::vector<double>>(in_height + 2 * padding, std::vector<double>(in_width + 2 * padding)));
    Tensor3d output(out_channel_num, std::vector<std::vector<double>>(out_height, std::vector<double>(out_width)));

    for (int c = 0; c < in_channel_num; ++c) {
        for (int w = 0; w < in_width; ++w) {
            for (int h = 0; h < in_height; ++h) {
                padded_input[c][w + padding][h + padding] = input[c][w][h];
            }
        }
    }


    for (int out_channel = 0; out_channel < out_channel_num; ++out_channel) {
        for (int h = 0; h < out_height; ++h) {
            for (int w = 0; w < out_width; ++w) {
                for (int in_channel = 0; in_channel < in_channel_num; ++in_channel) {
                    for (int kh = 0; kh < kernel_size; ++kh) {
                        for (int kw = 0; kw < kernel_size; ++kw) {
                            // Consider the stride
                            output[out_channel][h][w] += weights[out_channel][in_channel][kh][kw] * padded_input[in_channel][h * stride + kh][w * stride + kw];
                        }
                    }
                }
                output[out_channel][h][w] += biases[out_channel];
            }
        }
    }

    return output;
}

Tensor3d reluLayer3d(const Tensor3d& input) {
    Tensor3d relu_output = input;

    for (size_t c = 0; c < input.size(); ++c) {
        for (size_t h = 0; h < input[0].size(); ++h) {
            for (size_t w = 0; w < input[0][0].size(); ++w) {
                if (input[c][h][w] < 0) {
                    relu_output[c][h][w] = 0;
                }
            }
        }
    }

    return relu_output;
}

// Give me a relu layer 1d function
Tensor1d reluLayer1d(const Tensor1d& input) {
    Tensor1d relu_output = input;

    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] < 0) {
            relu_output[i] = 0;
        }
    }

    return relu_output;
}

Tensor3d maxpooling(const Tensor3d& input, int in_channel_num, int kernel_size, int stride) {

    // get in height from input
    int out_height = (input[0].size() - kernel_size) / stride + 1;
    int out_width = (input[0][0].size() - kernel_size) / stride + 1;

    Tensor3d output(in_channel_num, std::vector<std::vector<double>>(out_height, std::vector<double>(out_width)));

    for (int c = 0; c < in_channel_num; ++c) {
        for (int h = 0; h < out_height; ++h) {
            for (int w = 0; w < out_width; ++w) {
                double max_val = input[c][h * stride][w * stride];
                for (int kh = 0; kh < kernel_size; ++kh) {
                    for (int kw = 0; kw < kernel_size; ++kw) {
                        max_val = std::max(max_val, input[c][h * stride + kh][w * stride + kw]);
                    }
                }
                output[c][h][w] = max_val;
            }
        }
    }

    return output;
}

Tensor1d flattenTensor(const Tensor3d& tensor) {
    Tensor1d flattened_tensor;
    // Calculate the total number of elements in the 3D tensor
    size_t total_elements = tensor.size() * tensor[0].size() * tensor[0][0].size();
    // Reserve space in the flattened tensor to avoid reallocations
    flattened_tensor.reserve(total_elements);

    // Iterate over each element of the 3D tensor and append it to the flattened tensor
    for (const auto& slice : tensor) {
        for (const auto& row : slice) {
            for (const auto& element : row) {
                flattened_tensor.push_back(element);
            }
        }
    }

    return flattened_tensor;
}


// fully conneceted layer with input, weights and biases also input size output size as parameters
Tensor1d fc_layer(const Tensor1d& input, const Tensor2d& weights, const Tensor1d& biases, int input_size, int output_size) {
    Tensor1d output(output_size);

    for (int i = 0; i < output_size; ++i) {
        for (int j = 0; j < input_size; ++j) {
            // output[i] += input[j] * weights[i * input_size + j];
            output[i] += input[j] * weights[i][j];
        }
        output[i] += biases[i];
    }

    return output;
}

//Tensor 1d softmax function
Tensor1d softmax(const Tensor1d& input) {
    Tensor1d output(input.size());
    double sum = 0.0;

    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = std::exp(input[i]);
        sum += output[i];
    }

    for (size_t i = 0; i < input.size(); ++i) {
        output[i] /= sum;
    }

    return output;
}

// read in classes from a file, each line is a class
std::vector<std::string> readClasses(const std::string& file_directory) {
    std::ifstream inputFile(file_directory);
    std::vector<std::string> classes;
    std::string class_name;

    while (std::getline(inputFile, class_name)) {
        classes.push_back(class_name);
    }

    inputFile.close();

    return classes;
}


// map the softmax value to the corresponding class
std::string mapSoftmaxToClass(const Tensor1d& softmax, const std::vector<std::string>& classes) {
    double max_val = 0.0;
    int max_idx = 0;

    for (size_t i = 0; i < softmax.size(); ++i) {
        if (softmax[i] > max_val) {
            max_val = softmax[i];
            max_idx = i;
        }
    }

    return classes[max_idx];
}

// print out the classes and the corresponding softmax value
void printSoftmaxValues(const Tensor1d& softmax, const std::vector<std::string>& classes) {
    for (size_t i = 0; i < softmax.size(); ++i) {
        std::cout << classes[i] << ": " << softmax[i] << std::endl;
    }
}

// First map the softmax values to the corresponding class and then sort the classes based on the softmax values
std::vector<std::pair<std::string, double>> sortClassesBasedOnSoftmax(const Tensor1d& softmax, const std::vector<std::string>& classes) {
    std::vector<std::pair<std::string, double>> class_softmax_pairs;

    for (size_t i = 0; i < softmax.size(); ++i) {
        class_softmax_pairs.push_back(std::make_pair(classes[i], softmax[i]));
    }

    std::sort(class_softmax_pairs.begin(), class_softmax_pairs.end(), [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
        return a.second > b.second;
    });

    return class_softmax_pairs;
}
