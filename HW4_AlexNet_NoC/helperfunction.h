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

// give me a function which takes x,y,dest_x,dest_y as input and returns the direction up the updated x,y also the
// direction to traverse N,S,E,W,Eject

// 0: NORTH, 1: EAST, 2: SOUTH, 3: WEST, 4: Local
int get_direction(int x, int y)
{
    if (x == 0 && y == 0)
    {
        return CORE;
    }
    else if (x < 0)
    {
        return WEST;
    }
    else if (x > 0)
    {
        return EAST;
    }
    else if (y < 0)
    {
        return NORTH;
    }
    else if (y > 0)
    {
        return SOUTH;
    }

    return NORTH;
}

// new x,y, calculates the new x,y, displacement vector toward destination
void get_new_xy(int x, int y, int &new_x, int &new_y)
{
    if (x == 0 && y == 0)
    {
        new_x = x;
        new_y = y;
    }
    else if (x < 0)
    {
        new_x = x + 1;
        new_y = y;
    }
    else if (x > 0)
    {
        new_x = x - 1;
        new_y = y;
    }
    else if (y < 0)
    {
        new_x = x;
        new_y = y + 1;
    }
    else if (y > 0)
    {
        new_x = x;
        new_y = y - 1;
    }
}

#endif // HELPERFUNCTION_H