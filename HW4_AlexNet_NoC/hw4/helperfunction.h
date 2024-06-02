#ifndef HELPERFUNCTION_H
#define HELPERFUNCTION_H

#include <systemc>
#include <cstring>
#include "USER_DEFINED_PARAM.h"

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
void get_new_xy(int x, int y,int &new_x, int &new_y)
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