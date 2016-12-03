-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2015 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): xmikla10 ( Peter Miklanek )
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is
-------------------------------------------------------
	signal PC : std_logic_vector(11 downto 0);
	signal PCinc : std_logic;
	signal PCdec : std_logic;
-------------------------------------------------------
	signal PTR : std_logic_vector(11 downto 0);
	signal PTRinc : std_logic;
	signal PTRdec : std_logic;
-------------------------------------------------------	
	signal CNT : std_logic_vector(10 downto 0);
 	signal CNTset : std_logic;
	signal CNTinc : std_logic;
	signal CNTdec : std_logic;
-------------------------------------------------------
	signal TMP : std_logic_vector(7 downto 0);
	signal MUX_1 : std_logic;
	signal MUX_2 : std_logic_vector(2 downto 0);

	type CPUstates is (
						 I_FETCH, I_FETCH_WAIT, SKIP , I_EOF, Decode,
						 I_IDLE,
						 I_INC_PTR,
						 I_DEC_PTR,
						 I_INC_VAL, I_INC_VAL_WAIT, I_INC_VAL_2, I_INC_VAL_END,
						 I_DEC_VAL, I_DEC_VAL_WAIT, I_DEC_VAL_2, I_DEC_VAL_END, 
			      		 I_WHILE_START, I_WHILE_START_1, I_WHILE_START_2, I_WHILE_START_3,
			      		 I_WHILE_START_4, I_WHILE_START_5,
			      		 I_WHILE_END, I_WHILE_END_1, I_WHILE_END_2, I_WHILE_END_3,
			      		 I_WHILE_END_4, I_WHILE_END_5, I_WHILE_END_6, I_WHILE_END_7,
			      		 I_WRITE, I_WRITE_1, I_WRITE_2, I_WRITE_3,
			      		 I_READ, I_READ_1, I_READ_2,
			      		 I_SAVE_TO_TMP, I_SAVE_TO_TMP_2, I_SAVE_TO_TMP_3, I_SAVE_TO_TMP_END,
			      		 I_TMP_TO_SAVE, I_TMP_TO_SAVE_2);
  
  signal present_state : CPUstates;
  signal next_state : CPUstates;   
-- ----------------------------------------------------------------------------
begin
	--CPU
	CPU: process(CLK, RESET)
		begin
			if( RESET = '1' ) then
				PC <= ( others => '0' );
				PTR <= ( others => '0' );
				present_state <= I_IDLE;

			elsif ( CLK'event ) and ( CLK = '1' ) then
				if ( EN = '1' ) then
					present_state <= next_state;
				end if;
-- ------------PC----------------
				if ( PCinc = '1' ) and ( PCdec = '0' ) then
					PC <= PC + 1;
		        elsif ( PCinc = '0' ) and ( PCdec = '1' ) then
		            PC <= PC - 1;
		        end if;
-- ------------PTR----------------
				if ( PTRinc = '1' ) and ( PTRdec = '0' ) then
					PTR <= PTR + 1;
		        elsif ( PTRinc = '0' ) and ( PTRdec = '1' ) then
		            PTR <= PTR - 1;
		        end if;
-- ------------CNT----------------
		        if ( CNTset = '1' ) then
		        	CNT <= "00000000001";
		        else	
					if ( CNTinc = '1' ) and ( CNTdec = '0' ) then
						CNT <= CNT + 1;
			        elsif ( CNTinc = '0' ) and ( CNTdec = '1' ) then
			            CNT <= CNT - 1;
			        end if;		
			    end if;    
			end if;		    
		end process ;
-- -----------------------------------------------------------
MUX1: process(CLK, PTR, MUX_1, PC )
    begin
      if( MUX_1 = '0' ) then
          DATA_ADDR <= '0' & PC;		   	
      else
          DATA_ADDR <= '1' & PTR;
        end if;
    end process;       
-- -----------------------------------------------------------
MUX2: process(CLK, MUX_2, DATA_RDATA, IN_DATA, TMP)
	begin
		case ( MUX_2 ) is
			when "000" =>
				 DATA_WDATA <= DATA_RDATA + 1;
			when "001" =>
				 DATA_WDATA <= DATA_RDATA - 1;
			when "010" =>
				 DATA_WDATA <= IN_DATA;
			when "100" =>
				 DATA_WDATA <= TMP;
			when others =>
				 DATA_WDATA <= X"00"; 
		end case;
	end process;	
-- -----------------------------------------------------------
	state : process( present_state, DATA_RDATA, CNT, IN_VLD, IN_DATA, OUT_BUSY )
		begin
			PCinc <= '0';
			PCdec <= '0';
			PTRinc <= '0';
			PTRdec <= '0';
		 	CNTset <= '0';
			CNTinc <= '0';
			CNTdec <= '0';
			IN_REQ <= '0';
			OUT_WE <= '0'; 	  
			DATA_EN <= '0';

-- -------------------CASE-------------------------------------
			case (present_state) is
				when I_IDLE =>
					next_state <= I_FETCH;
-- -----------------------------------------------------------
				when I_FETCH =>
				    DATA_RDWR <= '1';
         			DATA_EN <= '1';
         			MUX_1 <= '0';
         			next_state <= I_FETCH_WAIT;

         			when I_FETCH_WAIT =>
         				next_state <= Decode;
-- -----------------------------------------------------------
         		when Decode =>
         			PCinc <= '1';
					PCdec <= '0';
         			case ( DATA_RDATA ) is
						when X"2B" =>
							 next_state <= I_INC_VAL;	-- +
						when X"2D" =>
							 next_state <= I_DEC_VAL; 	-- -
			 			when X"3E" =>
			 				 next_state <= I_INC_PTR; 	-- >
						when X"3C" =>
							 next_state <= I_DEC_PTR; 	-- <
						when X"5B" =>
							 next_state <= I_WHILE_START; 	-- [
						when X"5D" =>
							 next_state <= I_WHILE_END; 	-- ]
						when X"2E" =>
							 next_state <= I_WRITE; 	-- .
						when X"2C" =>
							 next_state <= I_READ; 	-- ,
						when X"24" =>
							 next_state <= I_SAVE_TO_TMP; -- $
						when X"21" =>
							 next_state <= I_TMP_TO_SAVE; -- !
						when X"00" =>
							 next_state <= I_EOF; 	-- EOF
						when others =>
							 next_state <= SKIP;	-- other
					end case;
-- -----------------------------------------------------------
				when SKIP => 		
					next_state <= I_FETCH;
-- -----------------------------------------------------------
				when I_EOF => 		
					next_state <= I_EOF;	
-- -----------------------------------------------------------
				when I_INC_PTR =>
						PTRinc <= '1';
						PTRdec <= '0';
						next_state <= I_FETCH;
-- -----------------------------------------------------------
				when I_DEC_PTR =>
						PTRdec <= '1';
						PTRinc <= '0';
						next_state <= I_FETCH;
-- -----------------------------------------------------------
				when I_INC_VAL =>
						MUX_1 <= '1';
						DATA_EN <= '1';
						DATA_RDWR <= '1';
						MUX_2 <= "000";
						next_state <= I_INC_VAL_WAIT;

						when I_INC_VAL_WAIT =>
							next_state <= I_INC_VAL_2;

							when I_INC_VAL_2 =>
								MUX_2 <= "000";
								DATA_EN <= '1';
								DATA_RDWR <= '0';
								MUX_1 <= '1';
								next_state <= I_INC_VAL_END;

								when I_INC_VAL_END =>
									MUX_2 <= "111";
									next_state <= I_FETCH;
-- -----------------------------------------------------------		
				when I_DEC_VAL =>
						MUX_1 <= '1';
						DATA_EN <= '1';
						DATA_RDWR <= '1';
						MUX_2 <= "001";
						next_state <= I_DEC_VAL_WAIT;

						when I_DEC_VAL_WAIT =>
							next_state <= I_DEC_VAL_2;

							when I_DEC_VAL_2 =>
								MUX_2 <= "001";
								DATA_EN <= '1';
								DATA_RDWR <= '0';
								MUX_1 <= '1';
								next_state <= I_DEC_VAL_END;

								when I_DEC_VAL_END =>
									MUX_2 <= "111";
									next_state <= I_FETCH;
-- -----------------------------------------------------------
    			when I_WRITE =>
			        MUX_1 <= '1';
			        DATA_RDWR <= '1';
			        DATA_EN <= '1';
			        next_state <= I_WRITE_1;

			    	when I_WRITE_1 =>
			         	if (OUT_BUSY = '1') then
			            	next_state <= I_WRITE_1;
			         	else
			            	next_state <= I_WRITE_2;
			         	end if;

			      		when I_WRITE_2 =>
			         		OUT_DATA <= DATA_RDATA;
			         		OUT_WE <= '1';
			         		next_state <= I_WRITE_3;

			      			when I_WRITE_3 =>
			         			next_state <= I_FETCH;
-- -----------------------------------------------------------				
			    when I_READ =>
			        IN_REQ <= '1';
			        next_state <= I_READ_1;

			    	when I_READ_1 =>
				        if (IN_VLD = '1') then
				            MUX_2 <= "010";
				            MUX_1 <= '1';
				            DATA_RDWR <= '0';
				            DATA_EN <= '1';
				            --IN_REQ <= '0';
				            next_state <= I_READ_2;
				        else 
				        	next_state <= I_READ_1;    
				        end if ;

				    	when I_READ_2 =>
				        	next_state <= I_FETCH;
-- -----------------------------------------------------------
			    when I_WHILE_START =>
		        	MUX_1 <= '1';
		            DATA_RDWR <= '1';
		            DATA_EN <= '1';
		            next_state <= I_WHILE_START_1;

		      		when I_WHILE_START_1 =>
		         		next_state <= I_WHILE_START_2;

		      			when I_WHILE_START_2 =>
		         			if ( DATA_RDATA = "00000000" ) then
		            			CNTset <= '1';
		            			next_state <= I_WHILE_START_3;
		         			else
		            			next_state <= I_FETCH;
		         			end if ;

		      			when I_WHILE_START_3 =>
		         			MUX_1 <= '0';
		         			DATA_RDWR <= '1';
		         			DATA_EN <= '1';
		         			PCinc <= '1';
		         			PCdec <= '0';
		         			next_state <= I_WHILE_START_4;

			      			when I_WHILE_START_4 =>
			         			next_state <= I_WHILE_START_5;

				      			when I_WHILE_START_5 =>
				         			if ( DATA_RDATA = X"5B" ) then --[
				            			CNTinc <= '1';
				            			CNTdec <= '0';
				            			next_state <= I_WHILE_START_3;
				         			elsif ( DATA_RDATA = X"5D" ) then --]
				            			CNTinc <= '0';
				            			CNTdec <= '1';
				            			if ( CNT = "000000001" ) then
				               				next_state <= I_FETCH;
				            			else
				               				next_state <= I_WHILE_START_3;
				            			end if;
				         			else
				            			next_state <= I_WHILE_START_3;
				         			end if ;			    
-- -----------------------------------------------------------
			    when I_WHILE_END =>
				    MUX_1 <= '1';
				    DATA_RDWR <= '1';
				    DATA_EN <= '1';
				    next_state <= I_WHILE_END_1;

				    	when I_WHILE_END_1 =>
				        	next_state <= I_WHILE_END_2;

					      	when I_WHILE_END_2 =>
					         	if ( DATA_RDATA = "00000000" ) then
					            	next_state <= I_FETCH;
					         	else
					            	PCinc <= '0';
					            	PCdec <= '1';
					            	CNTset <= '1';
					            	next_state <= I_WHILE_END_3;
					         	end if ;

							      	when I_WHILE_END_3 =>
							         	PCinc <= '0';
							         	PCdec <= '1';
							         	next_state <= I_WHILE_END_4;

					    when I_WHILE_END_4 =>
					        PCinc <= '0';
					        PCdec <= '1';
					        DATA_RDWR <= '1';
					        DATA_EN <= '1';
					        MUX_1 <= '0';
					        next_state <= I_WHILE_END_5;

					        when I_WHILE_END_5 =>
					            next_state <= I_WHILE_END_6;

						        when I_WHILE_END_6 =>
						         	if ( DATA_RDATA = X"5B" ) then  --[
						            	CNTinc <= '0';
						            	CNTdec <= '1';
							            if (CNT = "00000001") then
							               	CNTinc <= '1';
							               	CNTdec <= '0';
							              	PCinc <= '1';
							               	PCdec <= '0';
							               	next_state <= I_WHILE_END_7;
							            else
							              	 next_state <= I_WHILE_END_4;
							            end if;
						        	elsif ( DATA_RDATA = X"5D" ) then  --]
						            	CNTinc <= '1';
						           	 	CNTdec <= '0';
						            	next_state <= I_WHILE_END_4;
						         	else
						            	next_state <= I_WHILE_END_4;
						         	end if;

							      	when I_WHILE_END_7 =>
							         	next_state <= I_FETCH;

-- -----------------------------------------------------------
			    when I_SAVE_TO_TMP =>
					MUX_1 <= '1';
					DATA_EN <= '1';
					DATA_RDWR <= '1';
					next_state <= I_SAVE_TO_TMP_2;

		            when I_SAVE_TO_TMP_2 =>
		                next_state <= I_SAVE_TO_TMP_3;

			            when I_SAVE_TO_TMP_3 =>
			                MUX_1 <= '1';
			                TMP <= DATA_RDATA;
			                next_state <= I_SAVE_TO_TMP_END;

				            when I_SAVE_TO_TMP_END =>
				                next_state <= I_FETCH;
-- ----------------------------------------------------------!
			    when I_TMP_TO_SAVE =>
					MUX_1 <= '1';
					DATA_RDWR <= '0';
					DATA_EN <= '1';
					MUX_2 <= "100";
					next_state <= I_TMP_TO_SAVE_2;

						when I_TMP_TO_SAVE_2 =>
							next_state <= I_FETCH;

-- -----------------------------------------------------------
		end case;
	end process;
end behavioral; 