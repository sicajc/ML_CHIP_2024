#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"
#include "USER_DEFINED_PARAM.h"
#include "helperfunction.h"
#include "fifo.h"

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

    //=============================================================================
    // in fifos
    //=============================================================================
    // Declare fifo of size 5 for each direction, using primitive array
    // dont use interface or channel
    fifo in_fifo_north;
    fifo in_fifo_south;
    fifo in_fifo_east;
    fifo in_fifo_west;
    fifo in_fifo_core;

    // in fifo states
    int w_g_in_state = 0;
    int e_g_in_state = 0;
    int n_g_in_state = 0;
    int s_g_in_state = 0;
    int c_g_in_state = 0;

    //=============================================================================
    // in buffers
    //=============================================================================
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

    //=============================================================================
    // arbiters
    //=============================================================================
    // Arbiter inputs
    // 5 directions thus need 5 possible values
    int n_req_e = 0;
    int s_req_e = 0;
    int req_east = 0;
    int w_req_e = 0;
    int c_req_e = 0;

    // Arbiter outputs for east
    int e_grant_n = 0;
    int e_grant_s = 0;
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
        int port_selected;

        for (;;)
        {
            // core input fifo controller Global(G), Output Port selected(R), Also indicates if Empty or full
            switch (c_g_in_state)
            {
            case (G_IDLE):
                if (in_req[Core].read() == true && in_fifo_core.full == false)
                {
                    // send ack signal to the the core
                    out_ack[Core].write(true);

                    // if handshake is successful, then move to the next state and update fifo and states
                    while (in_req[Core].read() == false || out_ack[Core].read() == false)
                    {
                        // wait for the handshake to complete
                        cout << "Waiting for handshake to complete" << endl;
                        wait();
                    }
                    out_ack[Core].write(false);

                    // read in the header flit and decompose it to update the routing information
                    sc_lv<34> new_header = in_flit[Core].read();
                    int x_disp = in_flit[Core].read().range(23, 19).to_int();
                    int y_disp = in_flit[Core].read().range(18, 14).to_int();
                    int dst_x = in_flit[Core].read().range(13, 10).to_uint();
                    int dst_y = in_flit[Core].read().range(9, 6).to_uint();
                    int new_x;
                    int new_y;

                    port_selected = get_direction(x_disp, y_disp);
                    // get new direction, route the port, wait for the permission to the selected port granted
                    // compute new x,y
                    get_new_xy(x_disp, y_disp, new_x, new_y);

                    // display information
                    // cout << "=====================================================" << endl;
                    // cout << "Header flit: " << in_flit[Core].read() << endl;
                    // cout << "Source ID: " << in_flit[Core].read().range(31, 28).to_uint() << endl;
                    // cout << "Destination ID: " << in_flit[Core].read().range(27, 24).to_uint() << endl;
                    // cout << "Signed X displacement vector: " << x_disp << endl;
                    // cout << "Signed Y displacement vector: " << y_disp << endl;
                    // cout << "Destination X: " << dst_x << endl;
                    // cout << "Destination Y: " << dst_y << endl;
                    // cout << "New X: " << new_x << endl;
                    // cout << "New Y: " << new_y << endl;
                    // cout << "Port Selected: " << port_selected << endl;
                    // cout << "=====================================================" << endl;

                    // update the header's displacement vector
                    new_header.range(23, 19) = new_x;
                    new_header.range(18, 14) = new_y;

                    // Updates the states
                    c_g_in_state = G_ROUTING;

                    // put this header into fifo
                    in_fifo_core.flit_in(new_header);

                    // print fifo
                    in_fifo_core.print_fifo();
                }
                break;
            case (G_ROUTING):
                if (in_fifo_core.full == false)
                {
                    // 4 possible ports, for the selected port, route the flit to the port
                    // wait for request grant
                    switch (port_selected)
                    {
                    case North:
                        // c_req_n = 1;
                        break;
                    case East:
                        c_req_e = 1;
                        // wait for east grant west
                        while (e_grant_c == 0)
                        {
                            // wait for east grant west
                            cout << "Waiting for east grant core" << endl;
                            wait();
                        }
                        break;
                    case South:
                        // c_req_s = 1;
                        break;
                    case West:
                        // c_req_w = 1;
                        break;
                    default:
                        break;
                    }

                    c_g_in_state = G_ACTIVE;
                }
                break;
            case (G_ACTIVE):
                // if fifo is full keep waiting for available slots
                while(in_fifo_core.full == true)
                {
                    cout << "Fifo is full" << endl;
                    out_ack[Core].write(false);
                    wait();
                }
                out_ack[Core].write(true);

                // waiting for handshake to complete to the send resource to compelete
                while (out_ack[Core].read() == false || in_req[Core].read() == false)
                {
                    // wait for the handshake to complete
                    cout << "Waiting for handshake to complete" << endl;
                    wait();
                }

                cout << "out ack for core" << out_ack[Core].read() << endl;

                // send flit into fifo, sending data in fifo needs 1 cycle
                in_fifo_core.flit_in(in_flit[Core].read());
                wait();

                // take out values from fifo
                if(g_out_state_E == O_ACTIVE)
                {
                    in_fifo_core.flit_out();
                }

                c_g_in_state = G_ACTIVE;
                break;
            }
            // cout << "current state of core fifo : " << c_g_in_state << endl;
            in_fifo_core.print_fifo();
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
                if (e_grant_w == 1 || e_grant_n == 1 || e_grant_s == 1 || e_grant_c == 1)
                {
                    // if the arbiter grant one of the reqest, then move to the next state
                    g_out_state_E = O_ACTIVE;
                }
                break;
            case (O_ACTIVE):
                // permits sending data
                out_ack[East].write(1);
                // take data out from the granted source
                switch (e_out_source)
                {
                case West:
                    // out_buf_east = out_flit[West].read();
                    break;
                case North:
                    // out_buf_east = out_flit[North].read();
                    break;
                case South:
                    // out_buf_east = out_flit[South].read();
                    break;
                case Core:
                    out_buf_east = in_fifo_core.regs[0];
                    break;
                default:
                    break;
                }
                break;
            }
            // display current state of buffer
            // cout << "Current state of east buffer: " << g_out_state_E << endl;
            // cout << "current buffer data: " << out_buf_east << endl;
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

    SC_CTOR(Router)
    {
        SC_THREAD(core_in_fifo);
        sensitive << clk.pos();
        SC_THREAD(west_in_fifo);
        sensitive << clk.pos();
        SC_THREAD(east_out_buffer);
        sensitive << clk.pos();
        SC_THREAD(arbiter);
        sensitive << clk.pos();
    }
};

#endif