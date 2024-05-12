#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"

SC_MODULE(Router)
{
  sc_in<bool> rst;
  sc_in<bool> clk;

  sc_out<sc_lv<34>> out_flit[5];
  sc_out<bool> out_req[5];
  sc_in<bool> in_ack[5];

  sc_in<sc_lv<34>> in_flit[5];
  sc_in<bool> in_req[5];
  sc_out<bool> out_ack[5];

  // Used to indicate router id
  int id;
  // The dump file pointer, for core dumping
  sc_trace_file *tf;

  // out busy indicating available output port, whether the port is being occupied waiting for other route
  // to grant the access
  bool out_busy[5];
  // The output buffer data buffer
  sc_lv<34> p_data_buf[5];
  //  The current input and output direction decoded from the header
  int curr_in_dir[5], curr_out_dir[5];

  // SC_CTOR( Router ) {
  //     SC_METHOD( run );
  //     sensitive << clk.pos();
  // }

  // Same use the constructor type to allow dumping the waveform
  SC_CTOR(Router);
  Router(sc_module_name name, int id, sc_trace_file *tf = nullptr) : sc_module(name), id(id)
  {
    SC_METHOD(run);
    this->id = id;
    this->tf = tf;
    sensitive << clk.pos();
    dont_initialize();
    // std::cout << "Init: Router_" << id << std::endl;

    if (tf != nullptr)
    {
      sc_trace(tf, clk, "router_" + std::to_string(id) + ".clk");
      sc_trace(tf, rst, "router_" + std::to_string(id) + ".rst");

      // In order to dump the signals in a hierarchical way, we need to trace the signals in the router
      // The signals cannot be dumped in array format, one must seperate them out to dump the signals
      for (int i = 0; i < 5; i++)
      {
        sc_trace(tf, in_flit[i], "router_" + std::to_string(id) + ".in_flit_" + std::to_string(i));
        sc_trace(tf, in_req[i], "router_" + std::to_string(id) + ".in_req_" + std::to_string(i));
        sc_trace(tf, in_ack[i], "router_" + std::to_string(id) + ".in_ack_" + std::to_string(i));
        sc_trace(tf, out_flit[i], "router_" + std::to_string(id) + ".out_flit_" + std::to_string(i));
        sc_trace(tf, out_req[i], "router_" + std::to_string(id) + ".out_req_" + std::to_string(i));
        sc_trace(tf, out_ack[i], "router_" + std::to_string(id) + ".out_ack_" + std::to_string(i));
        sc_trace(tf, out_busy[i], "router_" + std::to_string(id) + ".out_busy_" + std::to_string(i));
      }
    }
  }

  int sclv4_2_int(const sc_lv<4> &lv)
  {
    int result = 0;
    for (int i = 3; i >= 0; --i)
    {
      result = (result << 1) + lv[i].to_bool();
    }
    return result;
  }

  // main logic of the router
  void run()
  {
    enum Direction
    {
      DIR_C = 0,
      DIR_E = 1,
      DIR_W = 2,
      DIR_S = 3,
      DIR_N = 4
    };
    // First reseting the router
    if (rst.read() == 1)
    {
      for (int i = 0; i < 5; i++)
      {
        // out_flit[i].write(0xf00 + i*16 + id); // for check connection
        out_flit[i].write(0);
        out_req[i].write(0);
        out_ack[i].write(0);
        curr_in_dir[i] = 0;
        curr_out_dir[i] = 0;
        out_busy[i] = false;
      }
    }
    else
    {
      // Look from the output buffer perspective, to see who needs the resource, this does not require arbitration
      for (int out_port = 0; out_port < 5; out_port++)
      {
        if (!out_busy[out_port]) // First check if the current output port is idle
        {
          // If it is idle, then check if there is a packet to read
          for (int in_port = 0; in_port < 5; in_port++)
          {
            if (in_req[in_port].read() == 1)
            {
              // there is a packet to read, put the packet onto the output buffer
              p_data_buf[out_port] = in_flit[in_port].read();
              // Check if this is a header, if not jump out the loop
              if (p_data_buf[out_port][33] != 1)
                continue; // not BOP, this immediatly skip the current iteration
              // update the destination id and  direction
              int dst_id = sclv4_2_int(p_data_buf[out_port].range(27, 24));
              // current id of router
              int id_x = id % 4;
              int id_y = id / 4;
              // dst_id from the header of the packet
              int dst_x = dst_id % 4;
              int dst_y = dst_id / 4;
              // choosing a certain direction
              int choice_dir;
              if (dst_id == id)
                // if the destination is the current router, then send to the core
                choice_dir = DIR_C;
              else if (dst_x == id_x)
                // if the destination is in the same column, then send to the north or south
                // ((4 + dst_y - id_y) % 4) <= 2 to prevent negative value, see which way is closer
                choice_dir = ((4 + dst_y - id_y) % 4) <= 2 ? DIR_S : DIR_N;
              else
                // if the destination is in the same row, then send to the east or west
                choice_dir = ((4 + dst_x - id_x) % 4) <= 2 ? DIR_E : DIR_W;
              // print id dst choice_dir
              // std::cout << "Router_" << id << " packet dst:" << dst_id << " choice_dir:" << choice_dir << std::endl;
              //
              if (choice_dir == out_port)
              {
                // Occupying the output port
                out_busy[out_port] = true;
                out_ack[in_port].write(1);
                out_flit[out_port].write(p_data_buf[out_port]);
                out_req[out_port].write(1);
                // The direction for this packet to go
                curr_out_dir[out_port] = choice_dir;
                curr_in_dir[out_port] = in_port;
                std::cout << "Router_" << id << " get packet (src:" << p_data_buf[out_port].range(31, 28) << " dst:" << p_data_buf[out_port].range(27, 24) << ")" << std::endl;
                break;
              }
            }
          }
        }
        else
        {
          // wait all the way until transmission is done
          // Wait for tail and the handshake acknowlege signal from the next router
          // This checks if the current output buffer has the tail
          if (p_data_buf[out_port][32] == 1 && in_ack[curr_out_dir[out_port]].read() == 1)
          { // EOP done
            out_busy[out_port] = false;                 // Release the output port
            out_req[curr_out_dir[out_port]].write(0);   // Stop the request
            out_flit[curr_out_dir[out_port]].write(0);  // Clear the current buffer
          }
          else if (in_ack[curr_out_dir[out_port]].read() == 1)
          {
            // Can send out the flit, due to handshake available, take the in_flit to the desired output buffer
            p_data_buf[out_port] = in_flit[curr_in_dir[out_port]].read();
            // put the packet to the output port
            out_flit[curr_out_dir[out_port]].write(p_data_buf[out_port]);
            out_req[curr_out_dir[out_port]].write(1);

            out_ack[curr_in_dir[out_port]].write(1);
          }
          else
          {
            out_ack[curr_in_dir[out_port]].write(0);
          }
        }
      }
    }
  }
};

#endif