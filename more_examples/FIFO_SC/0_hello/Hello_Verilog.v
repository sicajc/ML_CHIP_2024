`timescale 1ns/1ps
module Hello_Verilog(clk_pi);
  input clk_pi;
  always @(negedge clk_pi) begin :main_method
    $write("%t ps Hello Verilog\n",$time);
  end
endmodule
`define t_PERIOD 8
module verilog_main;
  reg clk;
  initial clk=1;
  always #(`t_PERIOD/2) clk <= ~clk;
  Hello_Verilog iHello_Verilog(
    .clk_pi(clk)
  );
  initial #10 $finish;
endmodule
