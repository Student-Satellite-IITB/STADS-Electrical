`timescale 1ns / 10ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 07/13/2020 01:31:06 PM
// Design Name: 
// Module Name: time_multiplexing_counter
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

// module for 4 segment time multiplexing circuit
module time_multiplexing_counter
(
input [3:0] in0,in1,in2,in3,
input wire clk,reset, 
output reg [7:0] sseg,
output reg [3:0] an
);
// constant declaration
// refreshing rate around clk frequency/2^(N-2) 
localparam N = 18;
//DESIGN THE PROPER ENABLE SIGNALS
reg [N-1:0] q_reg;
wire [N-1:0] q_next;
reg [3:0] bin_in;
//NEXT STATE LOGIC  
assign q_next = q_reg + 1;

//REGISTER
always @(posedge clk,posedge reset) begin
    if (reset)
        q_reg <= 0;
    q_reg <= q_next;
end

//ENABLE SIGNAL
always @* begin
    case (q_reg[N-1:N-2])
        2'b00:
            begin
            an = 4'b1110; //ACTIVE LOW ENABLE LEDs
            bin_in = in0;
            end
        2'b01:
            begin
            an = 4'b1101;
            bin_in = in1;
            end
        2'b10:
            begin
            an = 4'b1011;
            bin_in = in2;
            end
        2'b11:
            begin
            an = 4'b0111;
            bin_in = in3;
            end
    endcase       
end
//DEFINE MULTIPLEXER
always @* begin
    case (bin_in)
        0 : sseg[6:0] = 7'b0000001;
        1 : sseg[6:0] = 7'b1001111;
        2 : sseg[6:0] = 7'b0010010;
        3 : sseg[6:0] = 7'b0000110;
        4 : sseg[6:0] = 7'b1001100;
        5 : sseg[6:0] = 7'b0100100;
        6 : sseg[6:0] = 7'b0100000;
        7 : sseg[6:0] = 7'b0001111;
        8 : sseg[6:0] = 7'b0000000;
        9 : sseg[6:0] = 7'b0000100;
        default: sseg [6:0] = 7'b0111000; //4'hf
    endcase
    sseg [7] = 1'b0;
end

endmodule

// module for implementing a 4 digit counter
// CASCADE STRUCTURE IS USED 
module counter_4_BCD
(
input clk,enable,clr,
output wire [3:0] d0,d1,d2,d3
);
localparam mod = 5000000; // Assumming clk frequency to be 50MHz, this is the number of cycles to get 0.1s
reg [22:0] ms_reg; // 23 bit register required to generate 0.1 sec (reach the value 50*10^6)
wire [22:0] ms_next;
wire tick_0;
reg [3:0] d0_reg,d1_reg,d2_reg,d3_reg;
wire tick_1,tick_2,tick_3;  // tick_i enables ith digit
wire [3:0] d0_next,d1_next,d2_next,d3_next;

//REGISTER
always @(posedge clk) begin
    d0_reg <= d0_next;
    d1_reg <= d1_next;
    d2_reg <= d2_next;
    d3_reg <= d3_next;
end 

//NEXT STATE LOGIC
assign ms_next = (clr || ms_reg == mod) ? 4'b0:
                 (enable) ? ms_reg + 1 : ms_reg;
assign tick_0 = (ms_reg == mod) ? 1'b1:1'b0;

assign d0_next = (clr || tick_0 && d0_reg == 9) ?  4'b0:
                 (tick_0) ? d0_reg + 1:d0_reg;
assign tick_1 = (ms_reg == mod && d0_reg == 9) ? 1'b1:1'b0;

assign d1_next = (clr || tick_1 && d1_reg == 9) ?  4'b0:
                 (tick_1) ? d1_reg + 1:d1_reg;
assign tick_2 = (ms_reg == mod && d0_reg == 9 && d1_reg == 9) ? 1'b1:1'b0;

assign d2_next = (clr || tick_2 && d2_reg == 9) ?  4'b0:
                 (tick_2) ? d2_reg + 1:d2_reg;
assign tick_3 = (ms_reg == mod && d0_reg == 9 && d1_reg == 9 && d2_reg == 9) ? 1'b1:1'b0;

assign d3_next = (clr || tick_3 && d3_reg == 9) ?  4'b0:
                 (tick_3) ? d3_reg + 1:d3_reg;

// OUTPUT ASSIGNMENT
assign d0 = d0_reg;
assign d1 = d1_reg;
assign d2 = d2_reg;
assign d3 = d3_reg;

endmodule


module assemble_all
(
input clk,enable,clr,
output reg [7:0] sseg,
output reg [3:0] an
);
//INTER-CONNECTIONS
wire [3:0] d0,d1,d2,d3;

counter_4_BCD C4_BCD (.clk(clk),.enable(enable),.clr(clr),.d0(d0),.d1(d1),.d2(d2),.d3(d3));
time_multiplexing_counter TMC (.in0(d0),.in1(d1),.in2(d2),.in3(d3),.clk(clk),.sseg(sseg),.an(an),.reset(clr));

endmodule