#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"
#include "USER_DEFINED_PARAM.h"
#include "helperfunction.h"
// #include <math>
using namespace std;

SC_MODULE(Router)
{
    sc_in<bool> rst;
    sc_in<bool> clk;

    sc_out<flit_size_t> out_flit[5];

    // Hand shake signal for output channels
    sc_out<bool> out_req[5];
    sc_in<bool> in_ack[5];

    // In channels going into the buffers, once the buffer size is full, lock the buffer
    // give large size buffers first for funcionality
    sc_in<flit_size_t> in_flit[5];

    // Only when handshaking completes, allowing the flit to be transfered.
    sc_in<bool> in_req[5];
    sc_out<bool> out_ack[5];

    sc_signal<bool> out_buf_busy[5];
    sc_signal<bool> in_buf_busy[5];

    flit_size_t out_buf[5];
    flit_size_t in_buf[5];
    int src_current_dir[5]; // Used to store the current direction of the input buf
    int flit_sources[5];    // used to store the source of flit for each output ports

    int id;
    sc_trace_file *tf;

    void route_flits()
    {
        for (;;)
        {
            // reset
            if (rst.read() == true)
            {
                for (int i = 0; i < 5; i++)
                {
                    out_buf_busy[i] = false;
                    out_buf[i] = 0;
                    in_buf[i] = 0;
                    src_current_dir[i] = 0;
                    flit_sources[i] = 0;
                    in_buf_busy[i] = false;
                    //signal reset
                    out_flit[i].write(0);
                    out_req[i].write(false);
                    out_ack[i].write(false);
                }
            }
            else // not reset
            {
                // for every output ports
                // search for every input port
                for (int output_port_dir = 0; output_port_dir < 5; output_port_dir++)
                {
                    // if output port is busy, something to transfer
                    if (out_buf_busy[output_port_dir] == true)
                    {
                        out_req[output_port_dir].write(true);

                        flit_size_t output_port_flit = out_buf[output_port_dir];
                        int current_flit_source = flit_sources[output_port_dir];
                        out_flit[output_port_dir].write(out_buf[output_port_dir]);

                        // display flit sources for current router and its corresponding output port
                        //  router id info
                        //  cout << "==================Router id: " << id << endl;
                        //  cout << "Flit sources for current router: " << current_flit_source << " Output port: " << output_port_dir << endl;

                        // buffer holds tail and handshaked with output buffer
                        if ((output_port_flit[32] == 1) && in_ack[output_port_dir].read() == true)
                        {
                            // sends data out from the buffer
                            // release the output buffer
                            out_buf_busy[output_port_dir] = false;
                            out_req[output_port_dir].write(false);
                            out_flit[output_port_dir].write(0);
                            out_ack[current_flit_source].write(false);
                        }
                        else if (in_ack[output_port_dir] == true /*&& out_req[current_flit_source].read() == true*/)
                        {

                            // send the flit to the output port, note must delay out_ack by 1 cycle
                            out_buf[output_port_dir] = in_flit[current_flit_source].read();
                            out_flit[output_port_dir].write(out_buf[output_port_dir]);
                            out_ack[current_flit_source].write(true);

                            // cout << "Router " << id << " sent flit " << output_port_flit << " to port " << output_port_dir << endl;
                            // print current flit source and output dir
                            // cout << "Current flit source: " << current_flit_source << " Output port dir: " << output_port_dir << endl;
                        }
                        else {
                            out_ack[current_flit_source].write(false);
                        }
                    }
                    else // find new packet
                    {
                        for (int input_buf_dir = 0; input_buf_dir < 5; input_buf_dir++)
                        {
                            // check if there is packet to read
                            if (in_req[input_buf_dir] == true)
                            {

                                flit_size_t in_buf_flit = in_flit[input_buf_dir].read();
                                // if in_buf_flit is head
                                if (in_buf_flit[33] == 1)
                                {
                                    // Extract information
                                    int dst_id = in_buf_flit.range(27, 24).to_uint();

                                    int cur_x = this->id % 4;
                                    int cur_y = this->id / 4;

                                    int dst_x = dst_id % 4;
                                    int dst_y = dst_id / 4;

                                    int d_x = std::abs(cur_x - dst_x);
                                    int d_y = std::abs(cur_y - dst_y);

                                    int cur_dir;

                                    // finding the direction
                                    if (cur_x == dst_x && cur_y == dst_y)
                                    {
                                        // destination reached
                                        cur_dir = CORE;
                                    }
                                    else if (d_x != 0) // check if reached
                                    {
                                        // determine going east or west
                                        int cost_east = std::abs((4 + cur_x + 1) % 4 - dst_x);
                                        int cost_west = std::abs((4 + cur_x - 1) % 4 - dst_x);

                                        if (cost_east <= cost_west)
                                        {
                                            cur_dir = EAST;
                                        }
                                        else
                                        {
                                            cur_dir = WEST;
                                        }
                                    }
                                    else
                                    {
                                        // destermine going north or south
                                        int cost_north = std::abs((4 + cur_y - 1) % 4 - dst_y);
                                        int cost_south = std::abs((4 + cur_y + 1) % 4 - dst_y);

                                        if (cost_north <= cost_south)
                                        {
                                            cur_dir = NORTH;
                                        }
                                        else
                                        {
                                            cur_dir = SOUTH;
                                        }
                                    }

                                    // see if the selected port header matches the port direction
                                    if (cur_dir == output_port_dir && out_buf_busy[output_port_dir] == false)
                                    {
                                        // this shall be happened in the next cycle!
                                        out_buf_busy[output_port_dir] = true;
                                        flit_sources[output_port_dir] = input_buf_dir;

                                        // print time
                                        // cout << "================Time:  " << sc_time_stamp() <<"=================="<< endl;

                                        src_current_dir[input_buf_dir] = cur_dir;
                                        // send the head flit
                                        out_buf[output_port_dir] = in_flit[input_buf_dir].read();


                                        out_req[output_port_dir].write(true);
                                        out_flit[output_port_dir].write(out_buf[output_port_dir]);
                                        // Allows for next flit to be sent
                                        out_ack[input_buf_dir].write(true);

                                        // cout << "Router " << id << " sent flit " << out_buf[output_port_dir] << " to port " << output_port_dir << endl;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            wait();
        }
    }

    //=============================================================================
    // For File dumping & Constructor
    //=============================================================================
    SC_CTOR(Router);
    Router(sc_module_name name, int id, sc_trace_file *tf = nullptr) : sc_module(name), id(id)
    {
        SC_THREAD(route_flits);
        this->id = id;
        this->tf = tf;
        sensitive << clk.pos();
        dont_initialize();

        sc_trace(tf, clk, "router_" + std::to_string(id) + ".clk");
        sc_trace(tf, rst, "router_" + std::to_string(id) + ".rst");

        // In order to dump the signals in a hierarchical way, we need to trace the signals in the router
        // The signals cannot be dumped in array format, one must seperate them out to dump the signals
        for (int i = 0; i < 5; i++)
        {
            // I/Os
            sc_trace(tf, in_flit[i], "R_" + to_string(id) + ".in_flit_" + to_string(i));
            sc_trace(tf, in_req[i], "R_" + to_string(id) + ".in_req_" + to_string(i));
            sc_trace(tf, in_ack[i], "R_" + to_string(id) + ".in_ack_" + to_string(i));
            sc_trace(tf, out_flit[i], "R_" + to_string(id) + ".out_flit_" + to_string(i));
            sc_trace(tf, out_req[i], "R_" + to_string(id) + ".out_req_" + to_string(i));
            sc_trace(tf, out_ack[i], "R_" + to_string(id) + ".out_ack_" + to_string(i));
            // Inner signals
            sc_trace(tf, out_buf_busy[i], "R_" + to_string(id) + ".out_busy_" + to_string(i));
            sc_trace(tf, in_buf_busy[i], "R_" + to_string(id) + ".in_buf_busy_" + to_string(i));
            sc_trace(tf, out_buf[i], "R_" + to_string(id) + ".out_buf_" + to_string(i));
        }
    }
};

#endif