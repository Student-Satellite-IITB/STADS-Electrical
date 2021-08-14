----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 19.05.2020 16:59:36
-- Design Name: 
-- Module Name: adc_dac_interface - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;



entity adc_dac_interface is
 Port ( Clk : in std_logic; 
 reset: in std_logic;
 n_Intr_in: in std_logic;
 n_cs_out :out std_logic;
 n_wr_out: out std_logic;
 n_rd_out: out std_logic
 );
end  entity adc_dac_interface;

architecture Behavioral of adc_dac_interface is
component counter is
Port ( clk: in std_logic; 
    en : in std_logic;
          count_in: in std_logic_vector(4 downto 0);
        y: out std_logic );  
           end component counter ;


type fsm_states is (zero,one,two ,three,four);
signal state: fsm_states;
signal c1_out,c2_out,c3_out:std_logic;--out signals for c1, c2 ,c3
signal e1_s,e2_s,e3_s : std_logic := 'Z';--enable oins for c1,c2,c3

begin
 c1 : counter
 port map(clk=>Clk,en=>e1_s,count_in=>"00111",y=>c1_out);
 c2: counter 
 port map(clk=>Clk,en=>e2_s, count_in=>"01000",y=>c2_out);
 c3: counter
 port map(clk=>Clk,en=>e3_s,count_in=>"11111",y=>c3_out);
 


process(Clk,reset)

variable  e1,e2,e3: std_logic; 
variable next_state :fsm_states;
begin

e1:= e1_s;
e2:= e2_s;
e3:= e3_s;
next_state:= state;

 
 case state is
 when zero=>
 n_CS_out <= '1';
 n_WR_out<= '1';
 n_RD_out<='1';
 
 next_state:= one;
 
 when one=>
 n_CS_out<= '0';
 n_WR_out <='0';
 e1 :='1';
 if c1_out='1' then
 state<= two;
 e1:='0';
 else 
 next_state := one;
 end if;
 when two=>
 n_WR_out<='1';
 n_CS_out<='1';
 if (n_Intr_in = '0') then
 next_state := three;
 else 
 next_state := two;
 end if;
 when three=>
 n_RD_out<= '0';
 e2:='1';
 if c2_out='1' then
 next_state := four;
 e2:='0';
 else
 next_state:= three;
 end if;
 when four=>
 n_RD_out<='1';
 e3:= '1';
 if c3_out='1' then
 next_state := zero;
 e3:='0';
 else 
 next_state:= four;
 end if;
 end case;
  if rising_edge(Clk) then
  if reset='1' then
  state<=zero;
  else
  state<=next_state;
  e1_s<= e1;
  e2_s<= e2;
  e3_s<=e3;
  end if;
  end if;
  

 
 
end process;


 
 
end Behavioral;
