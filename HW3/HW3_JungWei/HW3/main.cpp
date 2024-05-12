#include "systemc.h"
#include "clockreset.h"
#include "core.h"
#include "router.h"

using namespace sc_core;

enum Direction {DIR_C = 0, DIR_E = 1, DIR_W = 2, DIR_S = 3, DIR_N = 4};
int sc_main(int argc, char* argv[])
{
  // =======================
  //   signals declaration
  // =======================
  sc_signal < bool > clk;
  sc_signal < bool > rst;

  sc_signal < sc_lv<34> > flit_rx;
  sc_signal < bool > req_rx;
  sc_signal < bool > ack_rx;

  sc_signal < sc_lv<34> > flit_tx;
  sc_signal < bool > req_tx;
  sc_signal < bool > ack_tx;

  //wire which connects router and router vertically, due to being torus architecture
  sc_signal < sc_lv<34> > wire_v_flit[4][4][2];// Having 4x4 wires, each wire has port, ack & req pairs
  sc_signal < bool > wire_v_req[4][4][2];
  sc_signal < bool > wire_v_ack[4][4][2];

  // wire which connects router and router horizontally, due to being torus architecture
  sc_signal < sc_lv<34> > wire_h_flit[4][4][2];
  sc_signal < bool > wire_h_req[4][4][2];
  sc_signal < bool > wire_h_ack[4][4][2];

  // wire which connects core and router
  sc_signal < sc_lv<34> > wire_c_flit[4][4][2];
  sc_signal < bool > wire_c_req[4][4][2];
  sc_signal < bool > wire_c_ack[4][4][2];

  // =======================
  //   waveform trace file for dumping signals
  // =======================
  sc_trace_file *tf = sc_create_vcd_trace_file("wave");
  sc_trace(tf, clk, "clk");
  sc_trace(tf, rst, "rst");

  // =======================
  //   modules declaration
  // =======================
  Clock m_clock("m_clock", 10);
  Reset m_reset("m_reset", 15);

  // Want to create 16 cores and 16 routers
  // This creates an 2D array of Core objects
  Core ***m_core;
  Router ***m_router;

  // allocates 4 pointers to create a two dimensional arrays
  m_core = (Core ***) malloc(4 * sizeof(Core **));
  m_router = (Router ***) malloc(4 * sizeof(Router **));

  for (int i = 0; i < 4; i++) {
    m_core[i] = (Core **) malloc(4 * sizeof(Core *));
    m_router[i] = (Router **) malloc(4 * sizeof(Router *));
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      int id = i * 4 + j;
      //  The constructor of core is defined as:
      //  Core(sc_module_name name, int id, sc_trace_file *tf = nullptr) : sc_module(name), id(id)
      //  This declaration allows assigning different cores different names
      m_core[i][j] = new Core(("m_core_" + std::to_string(id)).c_str(), id, tf);
      m_router[i][j] =  new Router(("m_router_" + std::to_string(id)).c_str(), id, tf);
    }
  }

  // =======================
  //   modules connection
  // =======================
  // Clock and Reset
  m_clock( clk );
  m_reset( rst );

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m_core[i][j]->clk( clk );
      m_core[i][j]->rst( rst );
      m_router[i][j]->clk( clk );
      m_router[i][j]->rst( rst );
    }
  }

  // Router <-> Core
  // Connecting m_core since it is an array, use in direct to connect ports
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m_core[i][j]->flit_rx( wire_c_flit[i][j][0] );
      m_core[i][j]->req_rx( wire_c_req[i][j][0] );
      m_core[i][j]->ack_rx( wire_c_ack[i][j][0] );

      m_core[i][j]->flit_tx( wire_c_flit[i][j][1] );
      m_core[i][j]->req_tx( wire_c_req[i][j][1] );
      m_core[i][j]->ack_tx( wire_c_ack[i][j][1] );

      m_router[i][j]->out_flit[DIR_C]( wire_c_flit[i][j][0] );
      m_router[i][j]->out_req[DIR_C]( wire_c_req[i][j][0] );
      m_router[i][j]->in_ack[DIR_C]( wire_c_ack[i][j][0] );

      m_router[i][j]->in_flit[DIR_C]( wire_c_flit[i][j][1] );
      m_router[i][j]->in_req[DIR_C]( wire_c_req[i][j][1] );
      m_router[i][j]->out_ack[DIR_C]( wire_c_ack[i][j][1] );
    }
  }

  // Router <-> Router (vertical)
  // Connecting m_router since it is an array, use in direct to connect ports
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m_router[i][j]->out_flit[DIR_S]( wire_v_flit[i][j][0] );
      m_router[i][j]->out_req[DIR_S]( wire_v_req[i][j][0] );
      m_router[i][j]->in_ack[DIR_S]( wire_v_ack[i][j][0] );

      m_router[i][j]->out_flit[DIR_N]( wire_v_flit[i][j][1] );
      m_router[i][j]->out_req[DIR_N]( wire_v_req[i][j][1] );
      m_router[i][j]->in_ack[DIR_N]( wire_v_ack[i][j][1] );

      m_router[i][j]->in_flit[DIR_S]( wire_v_flit[(i + 1) % 4][j][1] );
      m_router[i][j]->in_req[DIR_S]( wire_v_req[(i + 1) % 4][j][1] );
      m_router[i][j]->out_ack[DIR_S]( wire_v_ack[(i + 1) % 4][j][1] );

      m_router[i][j]->in_flit[DIR_N]( wire_v_flit[(i + 3) % 4][j][0] );
      m_router[i][j]->in_req[DIR_N]( wire_v_req[(i + 3) % 4][j][0] );
      m_router[i][j]->out_ack[DIR_N]( wire_v_ack[(i + 3) % 4][j][0] );
    }
  }

  // Router <-> Router (horizontal)
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      m_router[i][j]->out_flit[DIR_E]( wire_h_flit[i][j][0] );
      m_router[i][j]->out_req[DIR_E]( wire_h_req[i][j][0] );
      m_router[i][j]->in_ack[DIR_E]( wire_h_ack[i][j][0] );

      m_router[i][j]->out_flit[DIR_W]( wire_h_flit[i][j][1] );
      m_router[i][j]->out_req[DIR_W]( wire_h_req[i][j][1] );
      m_router[i][j]->in_ack[DIR_W]( wire_h_ack[i][j][1] );

      m_router[i][j]->in_flit[DIR_E]( wire_h_flit[i][(j + 1) % 4][1] );
      m_router[i][j]->in_req[DIR_E]( wire_h_req[i][(j + 1) % 4][1] );
      m_router[i][j]->out_ack[DIR_E]( wire_h_ack[i][(j + 1) % 4][1] );

      m_router[i][j]->in_flit[DIR_W]( wire_h_flit[i][(j + 3) % 4][0] );
      m_router[i][j]->in_req[DIR_W]( wire_h_req[i][(j + 3) % 4][0] );
      m_router[i][j]->out_ack[DIR_W]( wire_h_ack[i][(j + 3) % 4][0] );
    }
  }

  // =======================
  //   start simulation
  // =======================
  sc_start(1, SC_MS);

  // =======================
  //   end simulation
  // =======================
  sc_close_vcd_trace_file(tf);
  return 0;
}