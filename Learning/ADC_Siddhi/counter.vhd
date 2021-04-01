library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity counter is
    Port ( clk: in std_logic; 
    en : in std_logic;
          count_in: in std_logic_vector(4 downto 0);
        
           y: out std_logic ); 
end counter;

architecture Behavioral of counter is
signal count: std_logic_vector(4 downto 0):="00000" ;
begin

process(clk,count)
variable y_var: std_logic ;
begin
if(rising_edge(clk)) and en='1' then
    if count =count_in then
         count <= "00000" ;
         y_var:= '1';
    else
        count <= count + '1' ;
        y_var:='0';
    end if;
 end if;
 y<=y_var;
end process;

 
end Behavioral;