#ifndef ROM_H
#define ROM_H

#include "systemc.h"
#include <iostream>
#include <fstream>
using namespace std;

SC_MODULE(ROM)
{
    sc_in<bool> clk;
    sc_in<bool> rst;

    sc_in<int> layer_id;       // '0' means input data
    sc_in<bool> layer_id_type; // '0' means weight, '1' means bias (for layer_id == 0, we don't care this signal)
    sc_in<bool> layer_id_valid;

    sc_out<float> data;
    sc_out<bool> data_valid;

    void run();
    // vvv Please don't remove these two variables vvv
    string DATA_PATH;
    string IMAGE_FILE_NAME;
    // ^^^ Please don't remove these two variables ^^^

    SC_CTOR(ROM);

    ROM(sc_module_name name, sc_trace_file *tf = nullptr) : sc_module(name)
    {
        // Constructor
        DATA_PATH = "./data/";      // Please change this to your own data path
        IMAGE_FILE_NAME = "dog.txt"; // You can change this to test another image file

        SC_THREAD(run);
        sensitive << clk.pos() << rst.neg();

        // trace signals
        sc_trace(tf, rst, "m_ROM.rst");
        sc_trace(tf, clk, "m_ROM.clk");

        sc_trace(tf, layer_id, "m_ROM.layer_id");
        sc_trace(tf, layer_id_type, "m_ROM.layer_id_type");
        sc_trace(tf, layer_id_valid, "m_ROM.layer_id_valid");

        sc_trace(tf, data, "m_ROM.data");
        sc_trace(tf, data_valid, "m_ROM.data_valid");
    }
};

#endif