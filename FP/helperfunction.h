#ifndef HELPERFUNCTION_H
#define HELPERFUNCTION_H

#include <systemc>
#include <cstring>
#include "USER_DEFINED_PARAM.h"
#include "LAYER_PARAM.h"
#include "utils.h"

#include <vector>
typedef std::vector<float> Tensor1d;
typedef std::vector<std::vector<float>> Tensor2d;
typedef std::vector<std::vector<std::vector<float>>> Tensor3d;
typedef std::vector<std::vector<std::vector<std::vector<float>>>> Tensor4d;

//Input a 1d float img
float* assymetrical_padding(float* img)
{
    // convert tensor3dImgi to 3dtensor type
    Tensor3d tensor3dImg_i1 = convert1dTo3d(img, INPUT_IMG_CHANNEL, INPUT_IMG_WIDTH, INPUT_IMG_HEIGHT);
    // cout << "Conversion to 3d tensor" << endl;

    Tensor3d cat_img_padded(INPUT_IMG_CHANNEL, std::vector<std::vector<float>>(INPUT_IMG_WIDTH + 3, std::vector<float>(INPUT_IMG_HEIGHT + 3)));

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
    float *img_pad = convert3dTo1d(cat_img_padded, INPUT_IMG_CHANNEL, INPUT_IMG_WIDTH + 3, INPUT_IMG_HEIGHT + 3);

    return img_pad;
}

void print_sc_lv_as_float(sc_lv<32> val)
{
    uint32_t int_val = val.to_uint();
    float float_val;
    memcpy(&float_val, &int_val, sizeof(int_val));

    cout << "sc_lv<32> as float: " << float_val << endl;
}

// give me a function which converts float to sc_lv<32>
sc_lv<32> float_to_sc_lv(float val)
{
    uint32_t int_val;
    memcpy(&int_val, &val, sizeof(val));

    return int_val;
}

// give me a function which converts sc_lv<32> to float
float sc_lv_to_float(sc_lv<32> val)
{
    uint32_t int_val = val.to_uint();
    float float_val;
    memcpy(&float_val, &int_val, sizeof(int_val));

    return float_val;
}

#endif // HELPERFUNCTION_H