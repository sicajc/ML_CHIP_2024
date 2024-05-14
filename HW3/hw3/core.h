#ifndef CORE_H
#define CORE_H

#include "systemc.h"
#include "pe.h"

SC_MODULE(Core)
{
  sc_in<bool> rst;
  sc_in<bool> clk;
  // receive
  sc_in<sc_lv<34>> flit_rx; // The input channel
  sc_in<bool> req_rx;       // The request associated with the input channel
  sc_out<bool> ack_rx;      // The outgoing ack signal associated with the input channel
  // transmit
  sc_out<sc_lv<34>> flit_tx; // The output channel
  sc_out<bool> req_tx;       // The request associated with the output channel
  sc_in<bool> ack_tx;        // The outgoing ack signal associated with the output channel

  PE pe;
  int id;
  sc_trace_file *tf;

  Packet *p_tx, *p_rx;
  sc_lv<34> data_tx, data_rx;
  int data_tx_cnt;

  // SC_CTOR( Core ) {
  //     SC_METHOD( run );
  //     pe.init(id);
  //     sensitive << clk.pos();
  //     std::cout << "Core" << id << std::endl;
  // }

  SC_CTOR(Core);

  Core(sc_module_name name, int id, sc_trace_file *tf = nullptr) : sc_module(name), id(id)
  {
    SC_METHOD(run);
    pe.init(id);
    this->id = id;
    this->tf = tf;
    sensitive << clk.pos();
    dont_initialize();
    // std::cout << "Init: Core_" << id << std::endl;

    if (tf != nullptr)
    {
      sc_trace(tf, clk, "core_" + std::to_string(id) + ".clk");
      sc_trace(tf, rst, "core_" + std::to_string(id) + ".rst");
      sc_trace(tf, flit_rx, "core_" + std::to_string(id) + ".flit_rx");
      sc_trace(tf, req_rx, "core_" + std::to_string(id) + ".req_rx");
      sc_trace(tf, ack_rx, "core_" + std::to_string(id) + ".ack_rx");
      sc_trace(tf, flit_tx, "core_" + std::to_string(id) + ".flit_tx");
      sc_trace(tf, req_tx, "core_" + std::to_string(id) + ".req_tx");
      sc_trace(tf, ack_tx, "core_" + std::to_string(id) + ".ack_tx");
    }
  }

  sc_lv<32> float_2_sclv32(float number)
  {
    int *binary_representation = reinterpret_cast<int *>(&number);
    sc_lv<32> result;
    for (int i = 0; i < 32; ++i)
    {
      result[i] = (*binary_representation >> (31 - i)) & 0x1;
    }
    return result;
  }
  float sclv32_2_float(const sc_lv<32> &lv)
  {
    int int_representation = 0;
    for (int i = 0; i < 32; ++i)
    {
      int_representation = (int_representation << 1) + lv[i].to_bool();
    }
    float result;
    std::memcpy(&result, &int_representation, sizeof(float));
    return result;
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

  void run()
  {
    if (rst.read() == 1)
    {
      ack_rx.write(0);
      req_tx.write(0);
      flit_tx.write(0);
      // flit_tx.write(id); // for check connection
      p_tx = nullptr;
      p_rx = nullptr;
      data_tx_cnt = 0;
    }
    else
    {
      // transmit
      if (p_tx == nullptr)
      {
        flit_tx.write(0);
        req_tx.write(0);
        p_tx = pe.get_packet();
        data_tx_cnt = 0;
        if (p_tx != nullptr)
        {
          std::cout << "Info: Core_" << id << " get packet (src:"
                    << p_tx->source_id << " dst:" << p_tx->dest_id
                    << " size:" << p_tx->datas.size() << ")" << std::endl;
        }
      }
      else
      { // p_tx is not nullptr
        if (data_tx_cnt == 0)
        {
          data_tx[33] = 1; // BOP
          data_tx[32] = 0; // EOP
          data_tx.range(31, 28) = p_tx->source_id;
          data_tx.range(27, 24) = p_tx->dest_id;
          data_tx.range(23, 0) = 0;
          flit_tx.write(data_tx);
          req_tx.write(1);
          data_tx_cnt = data_tx_cnt + 1;
        }
        else if (ack_tx.read() == 1)
        {
          if (data_tx_cnt > p_tx->datas.size())
          {
            delete p_tx;
            p_tx = nullptr;
            req_tx.write(0);
            flit_tx.write(0);
          }
          // std::cout << "Core_" << id << " send data cnt:" << data_tx_cnt << std::endl;
          else if (data_tx_cnt == p_tx->datas.size())
          {
            data_tx[33] = 0; // BOP
            data_tx[32] = 1; // EOP
            data_tx.range(31, 0) = float_2_sclv32(p_tx->datas[data_tx_cnt - 1]);
            flit_tx.write(data_tx);
          }
          else
          {
            data_tx[33] = 0; // BOP
            data_tx[32] = 0; // EOP
            data_tx.range(31, 0) = float_2_sclv32(p_tx->datas[data_tx_cnt - 1]);
            flit_tx.write(data_tx);
          }
          data_tx_cnt = data_tx_cnt + 1;
        }
      }
      // receive
      if (ack_rx == 1)
      {
        ack_rx.write(0);
      }
      else if (req_rx.read() == 1)
      {
        data_rx = flit_rx.read();
        ack_rx.write(1);
        if (data_rx[33] == 1)
        { // BOP
          if (p_rx != nullptr)
            cout << "Core_" << id << " ERROR: BOP received while receiving packet" << endl;
          else
          {
            p_rx = new Packet();
            p_rx->source_id = sclv4_2_int(data_rx.range(31, 28));
            p_rx->dest_id = sclv4_2_int(data_rx.range(27, 24));
          }
        }
        else if (data_rx[32] == 1)
        { // EOP
          if (p_rx == nullptr)
            cout << "Core_" << id << " ERROR: EOP received while not receiving packet" << endl;
          else
          {
            p_rx->datas.push_back(sclv32_2_float(data_rx.range(31, 0)));
            // std::cout << "Core_" << id << " receive " \
            //           << sclv32_2_float(data_rx.range(31, 0)) << std::endl;

            // print p_rx packet
            std::cout << "Core_" << id << " check packet (src:"
                      << p_rx->source_id << " dst:" << p_rx->dest_id
                      << " size:" << p_rx->datas.size() << ")" << std::endl;
            for (int i = 0; i < p_rx->datas.size(); i++)
            {
              std::cout << "Core_" << id << " data[" << i << "]:" << p_rx->datas[i] << std::endl;
            }
            // check packet
            pe.check_packet(p_rx);
            delete p_rx;
            p_rx = nullptr;
          }
        }
        else
        {
          p_rx->datas.push_back(sclv32_2_float(data_rx.range(31, 0)));
          // std::cout << "Core_" << id << " receive " \
          //           << sclv32_2_float(data_rx.range(31, 0)) << std::endl;
        }
      }
      else
      {
        ack_rx.write(0);
      }
    }
    // Packet* p = pe.get_packet();
    //  if (p != NULL) {
    //      //pe.check_packet(p);
    //  }
  }
};

#endif