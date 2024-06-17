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

    void run()
    {
        int id;
        bool type;
        string filename;
        ifstream file;
        float value;
        bool is_reading_data = false;

        while (rst.read())
            wait();
        while (true)
        {
            if (!is_reading_data)
            {
                if (layer_id_valid.read())
                {
                    // Read signals
                    id = layer_id.read();
                    type = layer_id_type.read();
                    if (id == 0)
                    {
                        filename = string(DATA_PATH) + IMAGE_FILE_NAME;
                    }
                    else if (id <= 5)
                    {
                        if (type == 0)
                            filename = string(DATA_PATH) + "conv" + to_string(id) + "_weight.txt";
                        else
                            filename = string(DATA_PATH) + "conv" + to_string(id) + "_bias.txt";
                    }
                    else if (id <= 8)
                    {
                        if (type == 0)
                            filename = string(DATA_PATH) + "fc" + to_string(id) + "_weight.txt";
                        else
                            filename = string(DATA_PATH) + "fc" + to_string(id) + "_bias.txt";
                    }
                    else
                    {
                        cout << "Error: Invalid layer id " << id << "." << endl;
                        sc_stop();
                    }
                    file = ifstream(filename);
                    is_reading_data = true;
                }
            }
            else
            {
                if (layer_id_valid.read())
                {
                    cout << "Error: layer_id_valid should be low when reading data." << endl;
                    sc_stop();
                }
                data_valid.write(true);
                file >> value;
                if (file.eof())
                {
                    file.close();
                    data_valid.write(false);
                    data.write(0);
                    is_reading_data = false;
                    continue;
                }
                data.write(value);
            }
            wait();
        }
    }
    // vvv Please don't remove these two variables vvv
    string DATA_PATH;
    string IMAGE_FILE_NAME;
    // ^^^ Please don't remove these two variables ^^^

    SC_CTOR(ROM)
    {
        // Constructor
        DATA_PATH = "/home/2024MLChip/mlchip040/FP_3X3_VERSION/data/";       // Please change this to your own data path
        IMAGE_FILE_NAME = "cat.txt"; // You can change this to test another image file

        SC_THREAD(run);
        sensitive << clk.pos() << rst.neg();
    }
};

#endif