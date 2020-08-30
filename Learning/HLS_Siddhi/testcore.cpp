#include<stdio.h>
#include<iostream>
using namespace std;
typedef unsigned char u5;
void enpower_pl_counter(volatile bool run, volatile int delay, volatile u5 *counter,volatile u5 count);
void adc_dac_interface_hls(int n_intr,int n_cs,int n_rd,int n_wr,int n_reset);


int main(){
	volatile u5 *counter1;
	 int n_cs_t;
	 int n_rd_t;
	 int n_wr_t;
	int n_reset_t=0;
	int n_intr_t=1;
	 n_reset_t=1;
	n_intr_t=0;
	adc_dac_interface_hls( n_intr_t,n_cs_t,n_rd_t,n_wr_t,n_reset_t);


return 0;
}
