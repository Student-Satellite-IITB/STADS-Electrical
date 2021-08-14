`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 07/14/2020 12:13:34 PM
// Design Name: 
// Module Name: universal_binary_counter
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


module universal_binary_counter
#(parameter N=8)
(
input wire clk,reset,
input wire syn_clr,load,en,up,
input wire [N-1:0] d,
output wire max_tick, min_tick,
output wire [N-1:0] q
);
//DECLARE SIGNALS
reg [N-1:0] q_reg;
reg [N-1:0] q_next;

//REGISTER
always @(posedge clk,posedge reset) begin
    if (reset)
        q_reg = 0;
    else
        q_reg <= q_next;    
end

// NEXT STATE LOGIC
always @* begin
    if (syn_clr)
        q_next = 0;
    else if (load)   
        q_next = d;
    else if (en && up)
        q_next = q_reg + 1;
    else if (en && ~up)
        q_next = q_reg - 1;
    else
        q_next = q_reg;
end

// ASSIGN OUTPUT SIGNALS
assign max_tick = (q_reg == 2**N - 1) ? 1'b1:1'b0;
assign min_tick = (q_reg == 0) ? 1'b1:1'b0;
assign q = q_reg;

endmodule
