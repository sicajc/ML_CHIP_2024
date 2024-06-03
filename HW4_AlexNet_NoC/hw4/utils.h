#ifndef UTILS_H
#define UTILS_H

#include <systemc.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <LAYER_PARAM.h>

typedef std::vector<float> Tensor1d;
typedef std::vector<std::vector<float>> Tensor2d;
typedef std::vector<std::vector<std::vector<float>>> Tensor3d;
typedef std::vector<std::vector<std::vector<std::vector<float>>>> Tensor4d;

//Give me a function which converts 1d *float array to 3d Tensor
inline Tensor3d convert1dTo3d(float* input, int img_channel, int img_width, int img_height) {
    Tensor3d output(img_channel, std::vector<std::vector<float>>(img_width, std::vector<float>(img_height)));

    for (int c = 0; c < img_channel; ++c) {
        for (int w = 0; w < img_width; ++w) {
            for (int h = 0; h < img_height; ++h) {
                output[c][w][h] = input[c * img_width * img_height + w * img_height + h];
            }
        }
    }

    return output;
}

// Function converting 3d tensor to 1d float array
inline float* convert3dTo1d(const Tensor3d& input, int img_channel, int img_width, int img_height) {
    float* output = new float[img_channel * img_width * img_height];

    for (int c = 0; c < img_channel; ++c) {
        for (int w = 0; w < img_width; ++w) {
            for (int h = 0; h < img_height; ++h) {
                output[c * img_width * img_height + w * img_height + h] = input[c][w][h];
            }
        }
    }

    return output;
}

//Give me a function that converts 3d tensor to 1d float vector
inline Tensor1d convert3dTo1dVec(const Tensor3d& input, int img_channel, int img_width, int img_height) {
    Tensor1d output(img_channel * img_width * img_height);

    for (int c = 0; c < img_channel; ++c) {
        for (int w = 0; w < img_width; ++w) {
            for (int h = 0; h < img_height; ++h) {
                output[c * img_width * img_height + w * img_height + h] = input[c][w][h];
            }
        }
    }

    return output;
}

//Give me a function which resize 1d float weights to 4D weights
inline Tensor4d convert1dTo4d(float* input, int batch_size, int img_channel, int img_width, int img_height) {
    Tensor4d output(batch_size, std::vector<std::vector<std::vector<float>>>(img_channel, std::vector<std::vector<float>>(img_width, std::vector<float>(img_height))));

    for (int b = 0; b < batch_size; ++b) {
        for (int c = 0; c < img_channel; ++c) {
            for (int w = 0; w < img_width; ++w) {
                for (int h = 0; h < img_height; ++h) {
                    output[b][c][w][h] = input[b * img_channel * img_width * img_height + c * img_width * img_height + w * img_height + h];
                }
            }
        }
    }

    return output;
}

//Give me a function which converts 1d float bias to 1D tensor1d
inline Tensor1d convert1dToTensor1d(float* input, int tensor_size) {
    Tensor1d output(tensor_size);

    for (int i = 0; i < tensor_size; ++i) {
        output[i] = input[i];
    }

    return output;
}


//Give me a function which prints out a 1d float* data type array not Tensor1d
inline void display1DTensor(float* tensor, int tensor_size) {
    std::cout << "1D Tensor:\n";
    for (size_t i = 0; i < tensor_size; ++i) {
        std::cout << tensor[i] << " ";
    }
    std::cout << std::endl;
}


inline Tensor2d readAndResizeWeights2d(const std::string& file_directory, int rows, int cols) {
    std::ifstream inputFile(file_directory);
    float value;
    Tensor2d resized_data(rows, std::vector<float>(cols));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            inputFile >> value;
            resized_data[i][j] = value; // Assign the read value to the corresponding element in the 2D tensor
        }
    }

    inputFile.close();

    return resized_data;
}


inline void display1DTensorVec(const Tensor1d& tensor,int tensor_size) {
    std::cout << "1D Tensor:\n";
    for (size_t i = 0; i < tensor_size; ++i) {
        std::cout << tensor[i] << " ";
    }
    std::cout << std::endl;
}

// Function to display Tensor
inline void displayTensor3d(const Tensor3d& resized_data, int img_channel, int img_width, int img_height, int start_channel, int end_channel, int start_width, int end_width, int start_height, int end_height) {
    for (int c = start_channel; c < end_channel && c < img_channel; ++c) {
        std::cout << "Channel " << c << ":\n";
        for (int w = start_width; w < end_width && w < img_width; ++w) {
            for (int h = start_height; h < end_height && h < img_height; ++h) {
                std::cout << resized_data[c][w][h] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

inline void displayTensor4d(const Tensor4d& resized_data, int batch_size, int img_channel, int img_width, int img_height, int start_batch, int end_batch, int start_channel, int end_channel, int start_width, int end_width, int start_height, int end_height) {
    for (int b = start_batch; b < end_batch && b < batch_size; ++b) {
        std::cout << "Batch " << b << ":\n";
        for (int c = start_channel; c < end_channel && c < img_channel; ++c) {
            std::cout << "Channel " << c << ":\n";
            for (int w = start_width; w < end_width && w < img_width; ++w) {
                for (int h = start_height; h < end_height && h < img_height; ++h) {
                    std::cout << resized_data[b][c][w][h] << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
}

inline Tensor4d readAndResizeWeights4d(const std::string& file_directory, int batch_size, int img_channel, int img_width, int img_height) {
    std::ifstream inputFile(file_directory);
    float value;
    Tensor4d resized_data(batch_size, std::vector<std::vector<std::vector<float>>>(img_channel, std::vector<std::vector<float>>(img_width, std::vector<float>(img_height))));

    for (int b = 0; b < batch_size; ++b) {
        for (int c = 0; c < img_channel; ++c) {
            for (int w = 0; w < img_width; ++w) {
                for (int h = 0; h < img_height; ++h) {
                    inputFile >> value;
                    resized_data[b][c][w][h] = value; // Example resizing operation, you should replace it with your actual resizing logic
                }
            }
        }
    }

    inputFile.close();

    return resized_data;
}

// Function to read and resize 1D tensor
inline Tensor1d read1DTensor(const std::string& file_directory, int tensor_size) {
    std::ifstream inputFile(file_directory);
    float value;
    Tensor1d resized_data(tensor_size);

    for (int i = 0; i < tensor_size; ++i) {
        inputFile >> value;
        resized_data[i] = value; // Example resizing operation, you should replace it with your actual resizing logic
    }

    inputFile.close();

    return resized_data;
}


//Function to read and output a 1d float data type array
// Not tensor1d but float array
inline float* readAndResizeWeights1d(const std::string& file_directory, int tensor_size) {
    std::ifstream inputFile(file_directory);
    float* resized_data = new float[tensor_size];
    float value;

    for (int i = 0; i < tensor_size; ++i) {
        inputFile >> value;
        resized_data[i] = value; // Example resizing operation, you should replace it with your actual resizing logic
    }

    inputFile.close();

    return resized_data;
}

inline Tensor3d readAndResizeImageData3d(const std::string& file_directory, int img_channel, int img_width, int img_height) {
    std::ifstream inputFile(file_directory);
    float value;
    Tensor3d resized_data(img_channel, std::vector<std::vector<float>>(img_width, std::vector<float>(img_height)));

    for (int c = 0; c < img_channel; ++c) {
        for (int w = 0; w < img_width; ++w) {
            for (int h = 0; h < img_height; ++h) {
                inputFile >> value;
                resized_data[c][w][h] = value; // Example resizing operation, you should replace it with your actual resizing logic
            }
        }
    }

    inputFile.close();

    return resized_data;
}

// Give me function which converts it back
inline float* convertTensor1dTo1d(const Tensor1d& input, int tensor_size) {
    float* output = new float[tensor_size];

    for (int i = 0; i < tensor_size; ++i) {
        output[i] = input[i];
    }

    return output;
}



inline void releaseTensor4dMemory(Tensor4d& tensor) {
    for (auto& dim1 : tensor) {
        for (auto& dim2 : dim1) {
            for (auto& dim3 : dim2) {
                dim3.clear();
                dim3.shrink_to_fit();
            }
            dim2.clear();
            dim2.shrink_to_fit();
        }
        dim1.clear();
        dim1.shrink_to_fit();
    }
    tensor.clear();
    tensor.shrink_to_fit();
}

inline void releaseTensor3dMemory(Tensor3d& tensor) {
    for (auto& dim1 : tensor) {
        for (auto& dim2 : dim1) {
            dim2.clear();
            dim2.shrink_to_fit();
        }
        dim1.clear();
        dim1.shrink_to_fit();
    }
    tensor.clear();
    tensor.shrink_to_fit();
}

//relaseTensor2dMemory
inline void releaseTensor2dMemory(Tensor2d& tensor) {
    for (auto& dim1 : tensor) {
        dim1.clear();
        dim1.shrink_to_fit();
    }
    tensor.clear();
    tensor.shrink_to_fit();
}

inline void releaseTensor1dMemory(Tensor1d& tensor) {
    tensor.clear();
    tensor.shrink_to_fit();
}

#endif
