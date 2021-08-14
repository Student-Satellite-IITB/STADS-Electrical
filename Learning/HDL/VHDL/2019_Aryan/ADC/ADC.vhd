library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.std_logic_unsigned.all;
entity ADC is
   port (
         CS,RD,WR,Done: out std_logic;
			adc_out: in std_logic_vector(7 downto 0);
         clk, reset,INTR : in std_logic;
			dac_in: out std_logic_vector(7 downto 0));
end entity;
architecture Behave of ADC is
   -- Q
   type FsmState is (css,css2,css3,css4,css5,st, wrt, red,stay);
   signal fsm_state: FsmState;
   signal cn1out:std_logic;
	signal Count:integer:=0;
	signal timc:integer:=0;
	signal dac_prev:std_logic_vector(7 downto 0);
	signal t0,t1,t2,t3,t4,t5,t6,t7,t8:integer:=0;
component CountDownTimer is
   port (
         y: out std_logic;timer :out integer;
        Start, clk, reset : in std_logic);
end component;
begin
  coun1:CountDownTimer
        port map(y=>cn1out,timer=>timc,Start=>'1',reset=>INTR,clk=>clk);

  process(clk, reset, fsm_state,dac_prev,timc,t0,t1,t2,t3,t4,t5,t6,t7,t8)
    variable next_fsm_state_var: FsmState;
    variable Done_var: std_logic;
	 variable nq:integer;
	 variable nq1:integer;
	 variable v0,v1,v2,v3,v4,v5,v6,v7,v9:integer;
  begin
    next_fsm_state_var := fsm_state;
    Done_var := '0';
	 nq:=count;
	 nq1:=timc;
	 v0:=t0;v1:=t1;v2:=t2;v3:=t3;v4:=t4;v5:=t5;v6:=t6;v7:=t7;
	 case fsm_state is
       when st => 
                  next_fsm_state_var := css;
                  cs<='0';
						wr<='1';
						rd<='1';
						dac_in<=dac_prev;
       when css => 
            if (nq < 3) then
                  next_fsm_state_var := css;
                  cs<= '0';
                 else next_fsm_state_var := wrt;
					       WR<='0';
            end if;
		 when wrt => 
            if (nq < 9) then
                  next_fsm_state_var := wrt;
                  cs<= '0';
						WR<='0';
                 else next_fsm_state_var := css2;
					       WR<='1';
                      cs<='0';
            end if;
		 when css2 => 
            if (nq < 12) then
                  next_fsm_state_var := css2;
                  cs<= '0';
                 else next_fsm_state_var := css3;
                      cs<='1';
            end if;
		when css3 => 
            if (INTR='0') then
                  next_fsm_state_var := css4;
                  cs<= '0';
                 else next_fsm_state_var := css3;
                      cs<='1';
            end if;
		when css4 => 
            if (nq1<3) then
                  next_fsm_state_var := css4;
                 else next_fsm_state_var := red;
                      RD<='0';
            end if;
		when red => 
            if (nq1<16) then
                  next_fsm_state_var := red;
                 else next_fsm_state_var := css5;
                      RD<='1';
            end if;
		when css5 => 
            if (nq1<23 and INTR='0') then
				    if(nq1<21) then
					     t8<=to_integer(unsigned(adc_out));
				        --dac_prev<=adc_out;
					 end if;
                  next_fsm_state_var := css5;
                 else next_fsm_state_var := stay;
					       v0:=v1;v1:=v2;v2:=v3;v3:=v4;v4:=v5;v5:=v6;v6:=v7;v7:=t8;
						    v9:=(v0+v1+v2+v3+v4+v5+v6+v7)/8;	
							 dac_prev<=std_logic_vector(to_unsigned(v9,8));
                      cs<='1';
            end if;
       when stay => 
            if (nq<50000) then
                  next_fsm_state_var := stay;
                 else next_fsm_state_var := st;
                      nq:=0;
							 Done_var:='1';
            end if;
    end case;
    if(clk'event and clk='1') then
        if(reset = '1') then
           fsm_state <= st;
			  Count<=0;
        else
           fsm_state <= next_fsm_state_var; 
           nq:=nq+1;	
	        Count<=nq;	
		     Done<=Done_var;	 
			  t0<=v0;t1<=v1;t2<=v2;t3<=v3;t4<=v4;t5<=v5;t6<=v6;t7<=v7; 
        end if;
    end if;
    
  end process;

end Behave;