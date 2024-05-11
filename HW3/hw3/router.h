#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"
#include "USER_DEFINED_PARAM.h"
#include "fifo.h"
#include "fifo.cpp"

SC_MODULE(Router)
{
    sc_in<bool> rst;
    sc_in<bool> clk;

    // since having 5 directions, 0: North, 1: East, 2: South, 3: West, 4: Local
    // For arbitration, for each output ports
    // Allows East(E) & West(W) to go first then North(N) & South(S) eventually Local
    // Lock the output port once meeting the header, and unlock only after meeting the tail
    // The lock has to store the information of packet id, and the source id
    // Out channels
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

    // Encode
    // 0: North, 1: East, 2: South, 3: West, 4: Core
    const int North = 0;
    const int East = 1;
    const int South = 2;
    const int West = 3;
    const int Core = 4;

    // Declare fifo of size 5 for each direction, using primitive array
    // dont use interface or channel
    fifo in_fifo_north;
    fifo in_fifo_south;
    fifo in_fifo_east;
    fifo in_fifo_west;
    fifo in_fifo_core;

    // fifo control states (G)
    const int G_IDLE = 0;
    const int G_ROUTING = 1;
    const int G_WAITING_OUTPUT = 2;
    const int G_ACTIVE = 3;
    const int G_WAITING_ACK = 4;

    // in fifo states
    int w_g_in_state = 0;
    int e_g_in_state = 0;
    int n_g_in_state = 0;
    int s_g_in_state = 0;
    int c_g_in_state = 0;

    // output buffer
    sc_signal<flit_size_t> out_buf_north;
    sc_signal<flit_size_t> out_buf_south;
    sc_signal<flit_size_t> out_buf_east;
    sc_signal<flit_size_t> out_buf_west;
    sc_signal<flit_size_t> out_buf_core;

    int g_out_state_E = 0;
    int g_out_state_W = 0;
    int g_out_state_N = 0;
    int g_out_state_S = 0;
    int g_out_state_C = 0;

    // output buffer state
    const int O_IDLE = 0;
    const int O_WAITING_ACK = 1;
    const int O_ACTIVE = 2;
    const int O_DONE = 3;

    // Arbiter inputs
    // 5 directions thus need 5 possible values
    int n_req_e = 0;
    int s_req_e = 0;
    int req_east = 0;
    int w_req_e = 0;
    int c_req_e = 0;

    // Arbiter outputs
    int e_grant_n = 0;
    int e_grant_s = 0;
    int grant_east = 0;
    int e_grant_w = 0;
    int e_grant_c = 0;

    // Arbiter resource conditions
    int e_out_source = 0;
    int w_out_source = 0;
    int n_out_source = 0;
    int s_out_source = 0;
    int c_out_source = 0;

    void core_in_fifo()
    {
        for (;;)
        {
            // core input fifo controller Global(G), Output Port selected(R), Also indicates if Empty or full
            switch (c_g_in_state)
            {
            case (G_IDLE):
                if (in_req[Core].read() == 1 && in_fifo_core.full == false)
                {
                    c_g_in_state = G_ROUTING;
                }
                break;
            case (G_ROUTING):
                if (in_fifo_core.full == false)
                {
                    // Read in the header flit
                    in_fifo_core.flit_in(in_flit[Core].read());
                    c_g_in_state = G_WAITING_OUTPUT;
                }
                break;
            case (G_WAITING_OUTPUT):
                if (out_req[Core].read() == 1)
                {
                    c_g_in_state = G_ACTIVE;
                }
                break;
            case (G_ACTIVE):
                out_flit[Core].write(in_fifo_core.flit_out());
                out_ack[Core].write(1);
                c_g_in_state = G_WAITING_ACK;
                break;
            case (G_WAITING_ACK):
                if (in_ack[Core].read() == 1)
                {
                    c_g_in_state = G_IDLE;
                }
                break;
            }

            wait();
        }
    }

    // First code out west and east then test it
    void west_in_fifo()
    {
        for (;;)
        {
            // west input fifo controller Global(G), Output Port selected(R), Also indicates if Empty or full
            switch (w_g_in_state)
            {
            case (G_IDLE):
                if (in_req[West].read() == 1)
                {
                    w_g_in_state = G_ROUTING;
                }
                break;
            case (G_ROUTING):
                if (in_fifo_west.full == 0)
                {
                    in_fifo_west.flit_in(in_flit[West].read());
                    w_g_in_state = G_WAITING_OUTPUT;
                }
                break;
            case (G_WAITING_OUTPUT):
                if (out_req[West].read() == 1)
                {
                    w_g_in_state = G_ACTIVE;
                }
                break;
            case (G_ACTIVE):
                out_flit[West].write(in_fifo_west.flit_out());
                out_ack[West].write(1);
                w_g_in_state = G_WAITING_ACK;
                break;
            case (G_WAITING_ACK):
                if (in_ack[West].read() == 1)
                {
                    w_g_in_state = G_IDLE;
                }
                break;
            }

            wait();
        }
    }

    void east_out_buffer()
    {
        // Release the resource once the tail has been sent out
        for (;;)
        {
            // controller for output buffer
            switch (g_out_state_E)
            {
            case (O_IDLE):
                if (out_req[East].read() == 1)
                {
                    i_out_state = O_WAITING_ACK;
                }
                break;
            case (O_WAITING_ACK):
                if (in_ack[East].read() == 1)
                {
                    i_out_state = O_ACTIVE;
                }
                break;
            case (O_ACTIVE):
                out_ack[East].write(1);
                i_out_state = O_IDLE;
                break;
            }

            wait();
        }
    }

    void arbiter()
    {
        // monitors the resources and grants the resources to the output ports
        // 0 means idle, 1 means busy cannot be granted
        for (;;)
        {
            // Req -> monitor -> grant, do priority, east and west first then north and south
            // First check if the buffer is available
            // Then wait for the req, if req is 1, then grant the resource
            // The resource is locked until meeting the tail, it releases the resource
            // First monitor the East buffer
            // East output resource, 4 possible requesters, W,N,S,C
            // East has the highest priority
            // First test east side
            switch (g_out_state_E)
            {
            case (O_IDLE):
                // If west requesting east, this is set as the highest priority
                if (w_req_e == 1)
                {
                    if (e_grant_w == 0)
                    {
                        // grant resource tell the source that he gets the resource can start transfer
                        e_grant_w = 1;
                        // set the id to route to the port
                        e_out_source = West;
                    }
                }
                else if (n_req_e == 1)
                {
                    if (e_grant_n == 0)
                    {
                        e_grant_n = 1;
                        e_out_source = North;
                    }
                }
                else if (s_req_e == 1)
                {
                    if (e_grant_s == 0)
                    {
                        e_grant_s = 1;
                        e_out_source = South;
                    }
                }
                else if (c_req_e == 1)
                {
                    if (e_grant_c == 0)
                    {
                        e_grant_c = 1;
                        e_out_source = Core;
                    }
                }
                break;
            case (O_DONE):
                e_grant_w = 0;
                e_grant_n = 0;
                e_grant_s = 0;
                e_grant_c = 0;
                break;
            }

            // West output resource

            // North output resource

            // South output resource

            // Core output resource

            wait();
        }
    }
};

#endif