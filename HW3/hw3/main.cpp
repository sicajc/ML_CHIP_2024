#include "systemc.h"
#include "clockreset.h"
#include "USER_DEFINED_PARAM.h"
#include "core.h"
#include "pe.h"
#include "router.h"
#define SIM_TIME 250000

int sc_main(int argc, char *argv[])
{
    // =======================
    //   signals declaration
    // =======================
    sc_signal<bool> clk;
    sc_signal<bool> rst;

    sc_signal<sc_lv<34>> c_r_flit[4][4][2];
    sc_signal<bool> c_r_req[4][4][2];
    sc_signal<bool> c_r_ack[4][4][2];

    sc_signal<sc_lv<34>> r_r_flit_h[4][4][2];
    sc_signal<bool> r_r_req_h[4][4][2];
    sc_signal<bool> r_r_ack_h[4][4][2];

    sc_signal<sc_lv<34>> r_r_flit_v[4][4][2];
    sc_signal<bool> r_r_req_v[4][4][2];
    sc_signal<bool> r_r_ack_v[4][4][2];

    // Trace file declration for waveform dump
    sc_trace_file *tf = sc_create_vcd_trace_file("wave");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, rst, "rst");

    // =======================
    //   modules declaration
    // =======================
    Clock m_clock("m_clock", 10);
    Reset m_reset("m_reset", 15);

    // instantiate 16 cores and 16 routers as 4x4 array
    Core *m_core[4][4];
    Router *m_router[4][4];

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int id = i * 4 + j;
            // Router(sc_module_name name, int id, sc_trace_file *tf = nullptr) : sc_module(name), id(id)
            m_core[i][j] = new Core(("m_core_" + to_string(id)).c_str(), id, tf);
            m_router[i][j] = new Router(("m_router_" + to_string(id)).c_str(), id, tf);
        }
    }

    // =======================
    //   modules connection
    // =======================
    m_clock(clk);
    m_reset(rst);

    // connect all routers with cores
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // Connect rst and clk
            m_core[i][j]->clk(clk);
            m_core[i][j]->rst(rst);
            m_router[i][j]->clk(clk);
            m_router[i][j]->rst(rst);

            // Connect Core & routers
            m_core[i][j]->flit_rx(c_r_flit[i][j][0]);
            m_core[i][j]->req_rx(c_r_req[i][j][0]);
            m_core[i][j]->ack_rx(c_r_ack[i][j][0]);

            m_core[i][j]->flit_tx(c_r_flit[i][j][1]);
            m_core[i][j]->req_tx(c_r_req[i][j][1]);
            m_core[i][j]->ack_tx(c_r_ack[i][j][1]);

            m_router[i][j]->out_flit[CORE](c_r_flit[i][j][0]);
            m_router[i][j]->out_req[CORE](c_r_req[i][j][0]);
            m_router[i][j]->in_ack[CORE](c_r_ack[i][j][0]);

            m_router[i][j]->in_flit[CORE](c_r_flit[i][j][1]);
            m_router[i][j]->in_req[CORE](c_r_req[i][j][1]);
            m_router[i][j]->out_ack[CORE](c_r_ack[i][j][1]);

            // Connect Routers Vertically
            m_router[i][j]->out_flit[EAST](r_r_flit_h[i][j][0]);
            m_router[i][j]->out_req[EAST](r_r_req_h[i][j][0]);
            m_router[i][j]->in_ack[EAST](r_r_ack_h[i][j][0]);

            m_router[i][j]->out_flit[WEST](r_r_flit_h[i][j][1]);
            m_router[i][j]->out_req[WEST](r_r_req_h[i][j][1]);
            m_router[i][j]->in_ack[WEST](r_r_ack_h[i][j][1]);

            // Connect Routers Horizontally
            m_router[i][j]->in_flit[EAST](r_r_flit_h[i][(j + 1) % 4][1]);
            m_router[i][j]->in_req[EAST](r_r_req_h[i][(j + 1) % 4][1]);
            m_router[i][j]->out_ack[EAST](r_r_ack_h[i][(j + 1) % 4][1]);

            m_router[i][j]->in_flit[WEST](r_r_flit_h[i][(j + 3) % 4][0]);
            m_router[i][j]->in_req[WEST](r_r_req_h[i][(j + 3) % 4][0]);
            m_router[i][j]->out_ack[WEST](r_r_ack_h[i][(j + 3) % 4][0]);

            // Connect Routers Horizontally
            m_router[i][j]->out_flit[SOUTH](r_r_flit_v[i][j][0]);
            m_router[i][j]->out_req[SOUTH](r_r_req_v[i][j][0]);
            m_router[i][j]->in_ack[SOUTH](r_r_ack_v[i][j][0]);

            m_router[i][j]->out_flit[NORTH](r_r_flit_v[i][j][1]);
            m_router[i][j]->out_req[NORTH](r_r_req_v[i][j][1]);
            m_router[i][j]->in_ack[NORTH](r_r_ack_v[i][j][1]);

            m_router[i][j]->in_flit[SOUTH](r_r_flit_v[(i + 1) % 4][j][1]);
            m_router[i][j]->in_req[SOUTH](r_r_req_v[(i + 1) % 4][j][1]);
            m_router[i][j]->out_ack[SOUTH](r_r_ack_v[(i + 1) % 4][j][1]);

            m_router[i][j]->in_flit[NORTH](r_r_flit_v[(i + 3) % 4][j][0]);
            m_router[i][j]->in_req[NORTH](r_r_req_v[(i + 3) % 4][j][0]);
            m_router[i][j]->out_ack[NORTH](r_r_ack_v[(i + 3) % 4][j][0]);
        }
    }

    // set simulation end time
    sc_start(SIM_TIME, SC_NS);

    sc_close_vcd_trace_file(tf);
    return 0;
}