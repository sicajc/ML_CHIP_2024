#include "systemc.h"
#include "clockreset.h"
#include "USER_DEFINED_PARAM.h"
#include "core.h"
#include "pe.h"
#include "router.h"

int sc_main(int argc, char *argv[])
{
    // =======================
    //   signals declaration
    // =======================
    sc_signal<bool> clk;
    sc_signal<bool> rst;

    sc_signal<flit_size_t> router_i_flit_fifo[5];
    sc_signal<bool> router_i_req[5];
    sc_signal<bool> router_o_ack[5];

    sc_signal<flit_size_t> router_o_flit_fifo[5];
    sc_signal<bool> router_o_req[5];
    sc_signal<bool> router_i_ack[5];

    // =======================
    //   modules declaration
    // =======================
    Clock m_clock("m_clock", 10);
    Reset m_reset("m_reset", 15);
    CORE m_core("m_core");

    // router
    Router m_router("m_router");

    // =======================
    //   modules connection
    // =======================
    m_clock(clk);
    m_reset(rst);

    // All port must be bounded
    m_core.clk(clk);
    m_core.rst(rst);
    m_core.flit_rx(router_o_flit_fifo[Core]);
    m_core.req_rx(router_o_req[Core]);
    m_core.ack_tx(router_o_ack[Core]);

    m_core.flit_tx(router_i_flit_fifo[Core]);
    m_core.req_tx(router_i_req[Core]);
    m_core.ack_rx(router_i_ack[Core]);

    // connect router with core
    m_router.clk(clk);
    m_router.rst(rst);

    // connect router with core
    for (int i = 0; i < 5; i++)
    {
        m_router.out_flit[i](router_o_flit_fifo[i]);
        m_router.out_req[i](router_o_req[i]);
        m_router.out_ack[i](router_o_ack[i]);

        m_router.in_flit[i](router_i_flit_fifo[i]);
        m_router.in_req[i](router_i_req[i]);
        m_router.in_ack[i](router_i_ack[i]);
    }

    // set simulation end time
    // tracing
    sc_trace_file *tf = sc_create_vcd_trace_file("wave");
    sc_trace(tf, clk, "clk");
    sc_trace(tf, rst, "rst");
    sc_start(100, SC_NS);

    sc_close_vcd_trace_file(tf);
    return 0;
}