library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity CountDownTimer is
   port (
         y: out std_logic;
			timer :out integer;
        Start, clk, reset : in std_logic);
end entity;
architecture Behave of CountDownTimer is
   -- Q
   type FsmState is (IDLE, WORKS, DONES);
   signal fsm_state: FsmState;

   -- R
   signal R: std_logic;
	signal Count:integer;

begin

  process(clk, reset, fsm_state,R,Count, Start)
    variable next_fsm_state_var: FsmState;
    variable Done_var: std_logic;
	 variable nq:integer;
  begin
    next_fsm_state_var := fsm_state;
    Done_var := '0';
	 nq:=count;
    case fsm_state is
       when IDLE => 
            if (Start = '1') then
                  next_fsm_state_var := WORKS;
                  R <= '0';
               else next_fsm_state_var := IDLE;
            end if;
       when WORKS => 
            if (nq < 25) then
                  next_fsm_state_var := WORKS;
                 R <= '0';
                 else next_fsm_state_var := DONES;
					       R<='1';
                      y <= R;
            end if;
       when DONES => 
            Done_var := '1';
            next_fsm_state_var := IDLE;
				y<='0';
				---nq:=0;
    end case;
    if(clk'event and clk='1') then
        if(reset = '1') then
           fsm_state <= IDLE;
			  Count<=0;
        else
           fsm_state <= next_fsm_state_var; 
           nq:=nq+1;	
	        Count<=nq;	
		     timer<=Count;	  
        end if;
    end if;
  end process;

end Behave;