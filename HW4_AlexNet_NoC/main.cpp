#include "systemc.h"
#include "clockreset.h"
#include "USER_DEFINED_PARAM.h"
#include "core.h"
#include "router.h"
#include "controller.h"
#include "ROM.h"
using namespace std;
#define SIM_TIME 5000000

int sc_main(int argc, char *argv[])
{
    // =======================
    //   signals declaration
    // =======================
    sc_signal<bool> clk;
    sc_signal<bool> rst;

    sc_signal<int> layer_id;
    sc_signal<bool> layer_id_type;
    sc_signal<bool> layer_id_valid;

    sc_signal<float> data;
    sc_signal<bool> data_valid;

    sc_signal<sc_lv<34>> c_r_flit[4][4][2];
    sc_signal<bool> c_r_req[4][4][2];
    sc_signal<bool> c_r_ack[4][4][2];

    sc_signal<sc_lv<34>> r_r_flit_h[4][4][2];
    sc_signal<bool> r_r_req_h[4][4][2];
    sc_signal<bool> r_r_ack_h[4][4][2];

    sc_signal<sc_lv<34>> r_r_flit_v[4][4][2];
    sc_signal<bool> r_r_req_v[4][4][2];
    sc_signal<bool> r_r_ack_v[4][4][2];

    sc_signal<sc_lv<34>> dummy_flit[2];
    sc_signal<bool> dummy_req[2];
    sc_signal<bool> dummy_ack[2];

    // Trace file declration for waveform dump
    sc_trace_file *tf = nullptr;
    // sc_trace_file *tf = sc_create_vcd_trace_file("wave");
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
    Controller *m_controller;
    ROM *m_ROM;

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

    m_controller = new Controller(("m_controller"), tf);
    m_ROM = new ROM(("m_ROM"), tf);

    // =======================
    //   modules connection
    // =======================
    m_clock(clk);
    m_reset(rst);

    // ROM port connection
    m_ROM->rst(rst);
    m_ROM->clk(clk);

    m_ROM->layer_id(layer_id);
    m_ROM->layer_id_type(layer_id_type);
    m_ROM->layer_id_valid(layer_id_valid);

    m_ROM->data(data);
    m_ROM->data_valid(data_valid);

    // connect all controller routers and cores
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // Connect rst and clk
            m_core[i][j]->clk(clk);
            m_core[i][j]->rst(rst);
            m_router[i][j]->clk(clk);
            m_router[i][j]->rst(rst);

            if (i == 0 && j == 0)
            {
                // Core0 is reserved for Controller
                // Connect m_controller to Core
                m_controller->rst(rst);
                m_controller->clk(clk);

                // to ROM
                m_controller->layer_id(layer_id);
                m_controller->layer_id_type(layer_id_type);
                m_controller->layer_id_valid(layer_id_valid);

                // From ROM
                m_controller->data(data);
                m_controller->data_valid(data_valid);

                // From router0
                m_controller->flit_rx(c_r_flit[0][0][0]);
                m_controller->req_rx(c_r_req[0][0][0]);
                m_controller->ack_rx(c_r_ack[0][0][0]);

                // To router0
                m_controller->flit_tx(c_r_flit[0][0][1]);
                m_controller->req_tx(c_r_req[0][0][1]);
                m_controller->ack_tx(c_r_ack[0][0][1]);

                m_router[0][0]->out_flit[CORE](c_r_flit[i][j][0]);
                m_router[0][0]->out_req[CORE](c_r_req[i][j][0]);
                m_router[0][0]->in_ack[CORE](c_r_ack[i][j][0]);

                m_router[0][0]->in_flit[CORE](c_r_flit[i][j][1]);
                m_router[0][0]->in_req[CORE](c_r_req[i][j][1]);
                m_router[0][0]->out_ack[CORE](c_r_ack[i][j][1]);

                m_core[0][0]->flit_rx(dummy_flit[0]);
                m_core[0][0]->req_rx(dummy_req[0]);
                m_core[0][0]->ack_rx(dummy_ack[0]);

                m_core[0][0]->flit_tx(dummy_flit[1]);
                m_core[0][0]->req_tx(dummy_req[1]);
                m_core[0][0]->ack_tx(dummy_ack[1]);
            }
            else
            {

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
            }

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
    sc_start(SIM_TIME, SC_MS);

    sc_close_vcd_trace_file(tf);
    return 0;
}