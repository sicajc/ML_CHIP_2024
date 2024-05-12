#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"
#include "USER_DEFINED_PARAM.h"
#include "helperfunction.h"
#include "Fifo.h"

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
    // instantitate fifo with length 5
    Fifo<flit_size_t, BUFFER_SIZE> in_fifo_north;
    Fifo<flit_size_t, BUFFER_SIZE> in_fifo_south;
    Fifo<flit_size_t, BUFFER_SIZE> in_fifo_east;
    Fifo<flit_size_t, BUFFER_SIZE> in_fifo_west;
    Fifo<flit_size_t, BUFFER_SIZE> in_fifo_core;

    int in_fifo_north_ptr = 0;
    int in_fifo_south_ptr = 0;
    int in_fifo_east_ptr = 0;
    int in_fifo_west_ptr = 0;
    int in_fifo_core_ptr = 0;

    // in fifo states
    int w_g_in_state = 0;
    int e_g_in_state = 0;
    int n_g_in_state = 0;
    int s_g_in_state = 0;
    int c_g_in_state = 0;

    // in fifo tail flag
    int w_tail = 0;
    int e_tail = 0;
    int n_tail = 0;
    int s_tail = 0;
    int c_tail = 0;

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
        c_g_in_state = G_IDLE;

        for (;;)
        {
            // core input fifo controller Global(G), Output Port selected(R), Also indicates if Empty or full
            switch (c_g_in_state)
            {
            case (G_IDLE):
                // Wait for req from core
                while (in_req[Core].read() == false)
                {
                    wait();
                }
                // Send ack back to core
                out_ack[Core].write(true);
                // receive the header
                in_fifo_core.enqueue(in_flit[Core].read());

                // jump to Routing stage
                c_g_in_state = G_ROUTING;
                break;
            case (G_ROUTING):
            {
                int x_disp = in_flit[Core].read().range(23, 19).to_int();
                int y_disp = in_flit[Core].read().range(18, 14).to_int();
                int dst_x = in_flit[Core].read().range(13, 10).to_uint();
                int dst_y = in_flit[Core].read().range(9, 6).to_uint();
                int new_x;
                int new_y;

                // Check the front of fifo, see if it is header
                flit_size_t front_flit = in_fifo_core.front();

                // select the MSB of flit
                // int msb;
                // front_flit[33];
                int msb = front_flit[33] == 1;
                if (msb == 1) // means this is header
                {
                    // dequeue the fifo getting header
                    flit_size_t new_header = in_fifo_core.dequeue();
                    // check if the flit is header
                    new_header = front_flit;
                    // Decompose the header
                    // Determine the output port
                    port_selected = get_direction(x_disp, y_disp);
                    get_new_xy(x_disp, y_disp, new_x, new_y);

                    // update this new header
                    new_header.range(23, 19) = new_x;
                    new_header.range(18, 14) = new_y;

                    // Enqueue the data back to fifo
                    in_fifo_core.enqueue(new_header);
                }

                // Ask arbiter for the desired port, if permission granted leave the state
                // determine which port to go, and send request to that port, now only east port first
                c_req_e = 1;

                // If input handshake available and fifo is not full, put new data into fifo
                if (in_fifo_core.isFull() == false)
                {
                    out_ack[Core].write(false);
                }
                else if (in_req[Core].read() == true && out_ack[Core].read() == true)
                {
                    // put data into queue
                    in_fifo_core.enqueue(in_flit[Core].read());
                    out_ack[Core].write(true);
                }

                // wait for the request grantted by the arbiter
                if (c_req_e == 1 && e_grant_c == 1)
                {
                    c_g_in_state = G_ACTIVE;
                }
                else
                {
                    c_g_in_state = G_ROUTING;
                }

                // if the fifo request send the ack back to allow transfer
                if (in_req[Core].read() == true)
                {
                    out_ack[Core].write(true);
                }
                break;
            }
            case (G_ACTIVE):
            {
                // check output handshake, if available, send the data to the marked port
                // check if empty then send the req signal to the next router
                flit_size_t front_flit = in_fifo_core.front();

                c_tail = front_flit[32] == 1;
                // note since right now, the out_req is always false.
                if (in_fifo_core.isEmpty() == false && out_req[port_selected].read() == true)
                {
                    out_flit[port_selected].write(in_fifo_core.dequeue());
                    out_req[port_selected].write(true);
                }
                else
                {
                    out_req[port_selected].write(false);
                }

                // check input handshake, if input handshake is alright and fifo not full, then put the data into fifo
                if ((in_req[Core].read() == true && out_ack[Core].read() == true) && in_fifo_core.isFull() == false)
                {
                    in_fifo_core.enqueue(in_flit[Core].read());
                }

                // check if tail flit, if tail flit, then go back to idle and clear fifo status
                // Also needs to clear the arbiter status
                if (c_tail == 1)
                {
                    c_g_in_state = G_IDLE;
                }
                else
                {
                    c_g_in_state = G_ACTIVE;
                }

                // if the fifo request send the ack back to allow transfer
                if(in_fifo_core.isFull() == true)
                {
                    out_ack[Core].write(false);
                    cout << "Core FIFO IS FULL!" << endl;
                }
                else if (in_req[Core].read() == true)
                {
                    out_ack[Core].write(true);
                }

                break;
            }
            default:
            {
                cout << "No port selected in core input fifo" << endl;
                break;
            }
            }
            // display the current simulation time in systemC
            cout << "============" << sc_time_stamp() << "========================" << endl
                 << endl;
            cout << "current state of core fifo : " << c_g_in_state << endl;
            in_fifo_core.display();

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
            {
                // If west requesting east, this is set as the highest priority
                if (w_req_e == 1)
                {
                    if (e_grant_w == 0)
                    {
                        // grant resource tell the source that he gets the resource can start transfer
                        e_grant_w = 1;
                        // set the id to route to the port
                        e_out_source = West;
                        g_out_state_E = O_BUSY;
                    }
                }
                else if (n_req_e == 1)
                {
                    if (e_grant_n == 0)
                    {
                        e_grant_n = 1;
                        e_out_source = North;
                        g_out_state_E = O_BUSY;
                    }
                }
                else if (s_req_e == 1)
                {
                    if (e_grant_s == 0)
                    {
                        e_grant_s = 1;
                        e_out_source = South;
                        g_out_state_E = O_BUSY;
                    }
                }
                else if (c_req_e == 1)
                {
                    if (e_grant_c == 0)
                    {
                        e_grant_c = 1;
                        e_out_source = Core;
                        g_out_state_E = O_BUSY;
                    }
                }
                break;
            }
            case (O_BUSY):
            {
                switch (e_out_source)
                {
                case (North):
                {
                    if (n_tail == 1)
                    {
                        e_grant_n = 0;
                        g_out_state_E = O_IDLE;
                    }
                    break;
                }
                case (South):
                {
                    if (s_tail == 1)
                    {
                        e_grant_s = 0;
                        g_out_state_E = O_IDLE;
                    }
                    break;
                }
                case (West):
                {
                    if (w_tail == 1)
                    {
                        e_grant_w = 0;
                        g_out_state_E = O_IDLE;
                    }
                    break;
                }
                case (Core):
                {
                    if (c_tail == 1)
                    {
                        e_grant_c = 0;
                        g_out_state_E = O_IDLE;
                    }
                    break;
                }
                default:
                {
                    cout << "Error in East arbiter" << endl;
                    break;
                }
                }

                // display e_out_source
                cout << "current source of east arbiter : " << e_out_source << endl;
            }
            }

            // West output resource

            // North output resource

            // South output resource

            // Core output resource
            // display arbiter states
            cout << "current state of east arbiter : " << g_out_state_E << endl;

            wait();
        }
    }

    SC_CTOR(Router)
    {
        SC_THREAD(core_in_fifo);
        dont_initialize();
        sensitive << clk.pos();
        SC_THREAD(arbiter);
        dont_initialize();
        sensitive << clk.pos();
    }
};

#endif