//From the router.h systemC code, generate the I/O verilog i/o module for this router
// I mean router.h
module Router#(parameter id = 0)(
           input clk,
           input rst,

           // Output channels
           output reg[33:0] out_flit_N,
           output reg[33:0] out_flit_E,
           output reg[33:0] out_flit_S,
           output reg[33:0] out_flit_W,
           output reg[33:0] out_flit_Core,

           output reg out_req_N,
           output reg out_req_E,
           output reg out_req_S,
           output reg out_req_W,
           output reg out_req_Core,

           input in_ack_N,
           input in_ack_E,
           input in_ack_S,
           input in_ack_W,
           input in_ack_Core,

           // Input Channels
           input[33:0] in_flit_N,
           input[33:0] in_flit_E,
           input[33:0] in_flit_S,
           input[33:0] in_flit_W,
           input[33:0] in_flit_Core,

           input in_req_N,
           input in_req_E,
           input in_req_S,
           input in_req_W,
           input in_req_Core,

           output reg out_ack_N,
           output reg out_ack_E,
           output reg out_ack_S,
           output reg out_ack_W,
           output reg out_ack_Core
       );

reg[33:0] in_flit_ff[0:4];

reg out_req_ff[0:4];
reg out_req_wr[0:4];

reg out_ack_ff[0:4];
reg out_ack_wr[0:4];

reg in_req[0:4];

reg[33:0] out_flit_ff[0:4];
reg[33:0] out_flit_wr[0:4];

// Inner core
reg out_buf_busy_ff[0:4];
reg out_buf_busy_wr[0:4];

reg[33:0] out_buf_ff[0:4];
reg[33:0] out_buf_wr[0:4];

reg[31:0] src_current_dir_ff[0:4];
reg[31:0] src_current_dir_wr[0:4];

reg[31:0] flit_sources_ff[0:4];
reg[31:0] flit_sources_wr[0:4];

always @(*)
begin
    // Core 0, East 1, north 2,west 3,south 4
    in_flit[0] = in_flit_Core;
    in_flit[1] = in_flit_E;
    in_flit[2] = in_flit_N;
    in_flit[3] = in_flit_W;
    in_flit[4] = in_flit_S;

    in_req[0] = in_req_Core;
    in_req[1] = in_req_E;
    in_req[2] = in_req_N;
    in_req[3] = in_req_W;
    in_req[4] = in_req_S;

    out_flit_Core = out_flit_ff[0];
    out_flit_E = out_flit_ff[1];
    out_flit_N = out_flit_ff[2];
    out_flit_W = out_flit_ff[3];
    out_flit_S = out_flit_ff[4];

    out_req_Core = out_req_ff[0];
    out_req_E = out_req_ff[1];
    out_req_N = out_req_ff[2];
    out_req_W = out_req_ff[3];
    out_req_S = out_req_ff[4];

    out_ack_Core = out_ack_ff[0];
    out_ack_E = out_ack_ff[1];
    out_ack_N = out_ack_ff[2];
    out_ack_W = out_ack_ff[3];
    out_ack_S = out_ack_ff[4];
end

integer i;

always @(posedge clk)
begin
    if(rst)
    begin
        for(i=0;i<5;i=i+1)
        begin
            // reset all ff no wr
            out_flit_ff[i] <= 0;
            out_buf_busy_ff[i] <= 0;
            out_buf_ff[i] = 0;
            out_req_ff[i] <= 0;
            out_ack_ff[i] <= 0;
            src_current_dir_ff[i] <= 0;
            flit_sources_ff[i] <= 0;
        end
    end
    else
    begin
        for (i = 0; i<5; i=i+1)
        begin
            // connect all ff with wr
            out_flit_ff[i] <= out_flit_wr[i];
            out_buf_ff[i] <= out_buf_wr[i];
            out_req_ff[i] <= out_req_wr[i];
            out_ack_ff[i] <= out_ack_wr[i];
            out_buf_busy_ff[i] <= out_buf_busy_wr[i];
            src_current_dir_ff[i] <= src_current_dir_wr[i];
            flit_sources_ff[i] <= flit_sources_wr[i];
        end
    end
end

integer output_port_dir;
integer input_buf_dir;

reg[33:0] output_port_flit;
reg[31:0] current_flit_source;

reg[33:0] in_buf_flit;

// computing variables
reg[31:0] dst_id;
reg[31:0] cur_x,cur_y,dst_x,dst_y,d_x,d_y,cur_dir,cost_east,cost_west,cost_north,cost_south;

reg break_flag;

// Main compute logic
always @(*)
begin
    // connect wr to each ff, initialize the value
    for(i=0;i<5;i=i+1)
    begin
        // connect wr to each correspondent ff
        out_flit_wr[i] = out_flit_ff[i];
        out_req_wr[i] = out_req_ff[i];
        out_ack_wr[i] = out_ack_ff[i];
        out_buf_wr[i] = out_buf_ff[i];

        out_buf_busy_wr[i] = out_buf_busy_ff[i];
        src_current_dir_wr[i] = src_current_dir_ff[i];
        flit_sources_wr[i] = flit_sources_ff[i];
    end

    cur_x = id % 4;
    cur_y = id / 4;

    for(output_port_dir = 0; output_port_dir <5; output_port_dir = output_port_dir + 1)
    begin
        if(out_buf_busy_wr[output_port_dir] == 1) // output port is busy, things to transfer
        begin
            out_req_wr[output_port_dir] = 1;

            output_port_flit = out_buf_wr[output_port_dir];
            current_flit_source = flit_sources_wr[output_port_dir];

            out_flit_wr[output_port_dir] = out_buf_wr[output_port_dir];

            if((output_port_flit[32] == 1) && in_ack[output_port_dir] == 1)
            begin
                out_buf_busy_wr[output_port_dir] = 0;
                out_req_wr[output_port_dir] = 0;
                out_flit_wr[output_port_dir] = 0;
                out_ack_wr[output_port_dir] = 0;
            end
            else if(in_ack[output_port_dir] == 1)
            begin
                out_buf_wr[output_port_dir] = in_flit[current_flit_source];
                out_flit_wr[output_port_dir] = out_buf_wr[output_port_dir];
                out_ack_wr[current_flit_source] = 1;
            end
            else
            begin
                out_ack_wr[output_port_dir] = 0;
            end
        end
        else // find new packet
        begin
            break_flag = 0;
            for(input_buf_dir = 0; input_buf_dir < 5;input_buf_dir = input_buf_dir + 1)
            begin
                if(in_req[input_buf_dir] == 1 && break_flag != 1)
                begin
                    in_buf_flit = in_flit[input_buf_dir];

                    if(in_buf_flit[33] == 1)
                    begin
                        dst_id = in_buf_flit[27:24];

                        dst_x = dst_id %4;
                        dst_y = dst_id / 4;

                        d_x = abs_diff(cur_x-dst_x);
                        d_y = abs_diff(cur_y-dst_y);

                        if(cur_x == dst_x && cur_y == dst_y)
                        begin
                            // core
                            cur_dir = 0;
                        end
                        else if(d_x != 0)
                        begin
                            cost_east = abs_diff((4+cur_x+1)%4-dst_x);
                            cost_west = abs_diff((4+cur_x-1)%4-dst_x);

                            if(cost_east < cost_west)
                            begin
                                //east
                                cur_dir = 1;
                            end
                            else
                            begin
                                //west
                                cur_dir = 3;
                            end
                        end
                        else
                        begin
                            //determine going north or south
                            cost_north = abs_diff((4+cur_y-1)%4-dst_y);
                            cost_south = abs_diff((4+cur_y+1)%4-dst_y);

                            if(cost_north < cost_south)
                            begin
                                //north
                                cur_dir = 2;
                            end
                            else
                            begin
                                //south
                                cur_dir = 4;
                            end
                        end
                    end

                    // See if the port header matches the port direction also if it is free
                    if(cur_dir == output_port_dir && out_buf_busy_wr[output_port_dir] == 0)
                    begin
                        out_buf_busy_wr[output_port_dir] = 1;
                        flit_sources_wr[output_port_dir] = input_buf_dir;

                        src_current_dir_wr[output_port_dir] = cur_dir;

                        out_buf_wr[output_port_dir] = in_flit[input_buf_dir];

                        out_req_wr[output_port_dir] = 1;
                        out_flit_wr[output_port_dir] = out_buf_wr[output_port_dir];

                        out_ack_wr[input_buf_dir] = 1;
                        break_flag = 1;
                    end
                end
            end
        end
    end
end

// Give me a function which calculates the absolute value of two variables
function [31:0] abs_diff;
    input [31:0] a;
    input [31:0] b;
    begin
        if(a > b)
            abs_diff = a - b;
        else
            abs_diff = b - a;
    end
endfunction



endmodule