#ifndef HELPERFUNCTION_H
#define HELPERFUNCTION_H

#include <systemc>
#include <cstring>

void print_sc_lv_as_float(sc_lv<32> val) {
    uint32_t int_val = val.to_uint();
    float float_val;
    memcpy(&float_val, &int_val, sizeof(int_val));

    cout << "sc_lv<32> as float: " << float_val << endl;
}

// give me a function which converts float to sc_lv<32>
sc_lv<32> float_to_sc_lv(float val) {
    uint32_t int_val;
    memcpy(&int_val, &val, sizeof(val));

    return int_val;
}

// give me a function which converts sc_lv<32> to float
float sc_lv_to_float(sc_lv<32> val) {
    uint32_t int_val = val.to_uint();
    float float_val;
    memcpy(&float_val, &int_val, sizeof(int_val));

    return float_val;
}


#endif //HELPERFUNCTION_H