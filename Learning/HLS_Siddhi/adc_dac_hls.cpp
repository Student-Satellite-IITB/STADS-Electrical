#include<stdio.h>
#include<iostream>

using namespace std;
typedef unsigned char u5;
void enpower_pl_counter(volatile bool run, volatile int delay, volatile u5 *counter,volatile u5 count) ;


void adc_dac_interface_hls(int n_intr,int n_cs,int n_rd,int n_wr,int n_reset){
#pragma HLS INTERFACE s_axilite register port=n_intr
#pragma HLS INTERFACE s_axilite register port=n_cs
#pragma HLS INTERFACE s_axilite register port=n_rd
#pragma HLS INTERFACE s_axilite register port=n_wr
#pragma HLS INTERFACE s_axilite register port=n_reset
#pragma HLS INTERFACE s_axilite port=return
#pragma HLS pipeline II=1 enable_flush
typedef char fsm_state;
fsm_state state='0';
	 volatile u5 *counter1;
	if(n_reset==0){state='0';}
	else{
	switch(state){
	case '0':
		n_cs=1;
		n_wr=1;
		n_rd=1;
		state='1';
		break;
	case '1' :
		n_cs=0;
		n_wr=0;
		enpower_pl_counter(1, 0,counter1,7); //assuming 50MHZ clock of the fpga
		state='2';
		break;
	case '2' :
		n_wr=1;
		n_cs=1;
		if(n_intr==0){state='3';}
		else{state='2';}
		break;
	case '3' :
		n_rd=0;
		enpower_pl_counter(1,0,counter1,8);
		state='4';
		break;
	case '4' :
		n_rd=1;
		enpower_pl_counter(1,0,counter1,31);
		state='0';
		break;
	default :
		break;


	}

	}


}
void enpower_pl_counter(volatile bool run, volatile int delay, volatile u5 *counter,volatile u5 count) {
#pragma HLS INTERFACE ap_none register port=counter
#pragma HLS INTERFACE ap_none register port=delay
#pragma HLS INTERFACE ap_none register port=run
#pragma HLS RESOURCE variable=delay  core=AXI4LiteS metadata="-bus_bundle CONTROL_BUS"
#pragma HLS RESOURCE variable=return core=AXI4LiteS metadata="-bus_bundle CONTROL_BUS"

  volatile u5 counter_reg = 0;

  while(run == 1) {
    volatile int i = 0;
    for (i = 0; i < delay; i++);
    *counter = counter_reg++;
    if(*counter==count){run=0; *counter=0;}
  }
}
