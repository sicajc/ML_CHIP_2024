#include <systemc.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <LAYER_PARAM.h>

typedef std::vector<double> Tensor1d;
typedef std::vector<std::vector<double>> Tensor2d;
typedef std::vector<std::vector<std::vector<double>>> Tensor3d;
typedef std::vector<std::vector<std::vector<std::vector<double>>>> Tensor4d;

Tensor2d readAndResizeWeights2d(const std::string& file_directory, int rows, int cols) {
    std::ifstream inputFile(file_directory);
    double value;
    Tensor2d resized_data(rows, std::vector<double>(cols));

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            inputFile >> value;
            resized_data[i][j] = value; // Assign the read value to the corresponding element in the 2D tensor
        }
    }

    inputFile.close();

    return resized_data;
}


void display1DTensor(const Tensor1d& tensor,int tensor_size) {
    std::cout << "1D Tensor:\n";
    for (size_t i = 0; i < tensor_size; ++i) {
        std::cout << tensor[i] << " ";
    }
    std::cout << std::endl;
}

// Function to display Tensor
void displayTensor3d(const Tensor3d& resized_data, int img_channel, int img_width, int img_height, int start_channel, int end_channel, int start_width, int end_width, int start_height, int end_height) {
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

void displayTensor4d(const Tensor4d& resized_data, int batch_size, int img_channel, int img_width, int img_height, int start_batch, int end_batch, int start_channel, int end_channel, int start_width, int end_width, int start_height, int end_height) {
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

Tensor4d readAndResizeWeights4d(const std::string& file_directory, int batch_size, int img_channel, int img_width, int img_height) {
    std::ifstream inputFile(file_directory);
    double value;
    Tensor4d resized_data(batch_size, std::vector<std::vector<std::vector<double>>>(img_channel, std::vector<std::vector<double>>(img_width, std::vector<double>(img_height))));

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
Tensor1d read1DTensor(const std::string& file_directory, int tensor_size) {
    std::ifstream inputFile(file_directory);
    double value;
    Tensor1d resized_data(tensor_size);

    for (int i = 0; i < tensor_size; ++i) {
        inputFile >> value;
        resized_data[i] = value; // Example resizing operation, you should replace it with your actual resizing logic
    }

    inputFile.close();

    return resized_data;
}

Tensor3d readAndResizeImageData3d(const std::string& file_directory, int img_channel, int img_width, int img_height) {
    std::ifstream inputFile(file_directory);
    double value;
    Tensor3d resized_data(img_channel, std::vector<std::vector<double>>(img_width, std::vector<double>(img_height)));

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


void releaseTensor4dMemory(Tensor4d& tensor) {
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

void releaseTensor3dMemory(Tensor3d& tensor) {
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
void releaseTensor2dMemory(Tensor2d& tensor) {
    for (auto& dim1 : tensor) {
        dim1.clear();
        dim1.shrink_to_fit();
    }
    tensor.clear();
    tensor.shrink_to_fit();
}

void releaseTensor1dMemory(Tensor1d& tensor) {
    tensor.clear();
    tensor.shrink_to_fit();
}
