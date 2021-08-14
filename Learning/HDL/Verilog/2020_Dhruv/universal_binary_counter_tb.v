`timescale 1ns / 10ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 07/14/2020 12:30:45 PM
// Design Name: 
// Module Name: universal_binary_counter_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////
//module universal_binary_counter
//#(parameter N=8)
//(
//input wire clk,reset,
//input wire syn_clr,load,en,up,
//input wire [N-1:0] d,
//output wire max_tick, min_tick,
//output wire [N-1:0] q
//);

module universal_binary_counter_tb();
reg clk,reset,syn_clr,load,en,up;
localparam N = 3;
reg [N-1:0] d;
wire max_tick,min_tick;
wire [N-1:0] q;


// SET THE CLOCK - 20ns running forever
localparam T = 20;
always begin
    clk = 1'b1;
    #(T/2);
    clk = 1'b0;
    #(T/2);
end

universal_binary_counter #(.N(N)) uut (.clk(clk),.reset(reset),.syn_clr(syn_clr),.load(load),.en(en),.up(up),.d(d),
                                       .max_tick(max_tick),.min_tick(min_tick),.q(q));

//SET RESET PIN for first half cycle
initial begin
    reset = 1'b1;
    #(T/2);
    reset = 1'b0;
end

//other stimulus
initial begin
    //  ====initial inputs====
    syn_clr = 1'b0;
    load = 1'b0;
    d = 3'b000;
    en = 1'b0;
    up = 1'b1;
    @(negedge reset);   // wait for reset to deassert
    @(negedge clk);
    //  ===test load===
    load = 1'b1;
    d = 3'b110; // d = 6
    @(negedge clk) // wait for 1 clk cycle
    load = 1'b0;
    repeat(2) @(negedge clk);
    // ===test syn_clr===
    syn_clr = 1'b1;
    @(negedge clk);
    syn_clr = 1'b0;
    @(negedge clk);
    // ===test up counter and pause===
    en = 1'b1;
    up = 1'b1;
    repeat(10) @(negedge clk);
    en = 1'b0;
    repeat(2) @(negedge clk);
    en = 1'b1;
    up = 1'b0; // test down counter
    repeat(10) @(negedge clk);
    wait (q == 2); // wait statement
    @(negedge clk);
    wait (min_tick); // wait till min_tick is true 
    up = 1'b1;
    @(negedge clk);
    en = 1'b0; // pause
end
    
endmodule
