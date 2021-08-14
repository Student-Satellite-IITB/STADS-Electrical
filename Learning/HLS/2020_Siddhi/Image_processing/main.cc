/*
 * Commands to download memory to file (Download 76800 bytes from address 0x1300000 to file logresp.txt
 * set logfile [open "E:\\Lab_9_Matlab\\log.txt" "w"]
 * puts $logfile [mrd 0x1300000 76800 b]
 * close $logfile
 *
 *
 * */

#include <stdio.h>
#include "xaxidma.h"
#include "xdoHist.h"
#include "xdoHistStretch.h"
#include "LenaOnCode.h"
#include "AxiTimerHelper.h"

#define SIZE_ARR (320*240)


// Memory used by DMA
#define MEM_BASE_ADDR 	0x01000000
#define TX_BUFFER_BASE	(MEM_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE	(MEM_BASE_ADDR + 0x00300000)
// Get a pointer to the TX and RX dma buffer (CONFIGURE DMA)
// The pointers are for 8-bit memory but their addresses are 32 bit (u32)
unsigned char *m_dma_buffer_TX = (unsigned char*)TX_BUFFER_BASE;
unsigned char *m_dma_buffer_RX = (unsigned char*)RX_BUFFER_BASE;

unsigned int hist_sw[256];
unsigned char imgOut[SIZE_ARR];
unsigned char imgIn_HW[SIZE_ARR];

//Adress to BRAM controller
unsigned int *hist_hw =(unsigned int *) 0x40000000;


XAxiDma axiDma;
int initDMA()
{
	XAxiDma_Config *CfgPtr;
	CfgPtr = XAxiDma_LookupConfig(XPAR_AXI_DMA_0_DEVICE_ID);
	XAxiDma_CfgInitialize(&axiDma,CfgPtr);

	// Disable interrupts
	XAxiDma_IntrDisable(&axiDma,XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&axiDma,XAXIDMA_IRQ_ALL_MASK,XAXIDMA_DMA_TO_DEVICE);

	return XST_SUCCESS;
}

XDohist doHist;
XDohiststretch doHistStretch;
int initDoHist()
{
	int status;

	XDohist_Config *doHist_cfg;
	doHist_cfg = XDohist_LookupConfig(XPAR_DOHIST_0_DEVICE_ID);
	if (!doHist_cfg)
	{
		printf("Error loading config for doHist_cfg\n");
	}
	status = XDohist_CfgInitialize(&doHist,doHist_cfg);
	if (status != XST_SUCCESS)
	{
		printf("Error initializing for doHist\n");
	}

	return status;
}
int initDoHistStretch()
{
	int status;

	XDohiststretch_Config *doHistStretch_cfg;
	doHistStretch_cfg = XDohiststretch_LookupConfig(XPAR_DOHISTSTRETCH_0_DEVICE_ID);
	if (!doHistStretch_cfg)
	{
		printf("Error loading config for doHistStretch_cfg\n");
	}
	status = XDohiststretch_CfgInitialize(&doHistStretch,doHistStretch_cfg);
	if (status != XST_SUCCESS)
	{
		printf("Error initializing for doHistStretch\n");
	}

	return status;
}
void doHistSW(unsigned char *img,unsigned int *hist)
{
	for(int i=0;i<(320*240);i++){

		hist[i]=0;
	}
	for(int i=0;i<(320*240);i++){

			hist[img[i]]=hist[img[i]]+1;
		}


}
void doHistStretchSW(unsigned char *imgIn,unsigned char *imgOut,unsigned char xMin, unsigned char xMax)
{
	float xMax_minus_xMin=xMax-xMin;
	for(int i=0;i<(320*240);i++){

		float y_t_float=((imgIn[i]-xMin)/(xMax_minus_xMin))*255;
		imgOut[i]=y_t_float;
			}

}


int main()
{
	initDMA();
	initDoHist();
	initDoHistStretch();
	AxiTimerHelper axiTimer;

	printf("Doing histogram on SW\n");
	axiTimer.startTimer();
	doHistSW(img,hist_sw);
	axiTimer.stopTimer();
	double hist_SW_elapsed=axiTimer.getElapsedTimerInSeconds();
	printf("histogram SW execution time: %f sec\n",hist_SW_elapsed);
//get min value
	unsigned char xMin;
	for(int i=0;i< 256;i++){
		xMin=i;
		if(hist_sw[i])
			break;
	}
	//get max value
	unsigned char xMax;
	for(int i=255;i>=0;i--){
		xMax=i;
		if(hist_sw[i])
			break;

	}
	printf("doing histogram stretch SW\n");

	axiTimer.startTimer();
		doHistStretchSW(img,imgOut,xMin,xMax);
		axiTimer.stopTimer();
		double histstretch_SW_elapsed=axiTimer.getElapsedTimerInSeconds();
		printf("histogram SW execution time: %f sec\n",histstretch_SW_elapsed);

	// Populate data (Get image from header and put on memory)
	for (int idx = 0; idx < SIZE_ARR; idx++)
	{
		imgIn_HW[idx] = img[idx];
	}

	XDohist_Start(&doHist);
	//dummy values
	XDohiststretch_Set_xMin(&doHistStretch,255);
	XDohiststretch_Set_xMax(&doHistStretch,0);
XDohiststretch_Start(&doHistStretch);

	// Do the DMA transfer to push and get our image
	axiTimer.startTimer();
	Xil_DCacheFlushRange((u32)imgIn_HW,SIZE_ARR*sizeof(unsigned char));
	Xil_DCacheFlushRange((u32)m_dma_buffer_RX,SIZE_ARR*sizeof(unsigned char));

	XAxiDma_SimpleTransfer(&axiDma,(u32)imgIn_HW,SIZE_ARR*sizeof(unsigned char),XAXIDMA_DMA_TO_DEVICE);
	XAxiDma_SimpleTransfer(&axiDma,(u32)m_dma_buffer_RX,SIZE_ARR*sizeof(unsigned char),XAXIDMA_DEVICE_TO_DMA);

	//Wait transfers to finish
	while(XAxiDma_Busy(&axiDma,XAXIDMA_DMA_TO_DEVICE));
	while(XAxiDma_Busy(&axiDma,XAXIDMA_DEVICE_TO_DMA));

	// Invalidate the cache to avoid reading garbage
	Xil_DCacheInvalidateRange((u32)m_dma_buffer_RX,SIZE_ARR*sizeof(unsigned char));
	axiTimer.stopTimer();

	double hist_HW_elapsed = axiTimer.getElapsedTimerInSeconds();
	printf(" hist HW execution time: %f sec\n", hist_HW_elapsed);
//get min value
	for(int i=0;i< 256;i++){
			xMin=i;
			if(hist_hw[i])
				break;
		}
	//get max value

		for(int i=255;i>=0;i--){
			xMax=i;
			if(hist_hw[i])
				break;
		}

		XDohist_Start(&doHist);
			//Real values
			XDohiststretch_Set_xMin(&doHistStretch,xMax);
			XDohiststretch_Set_xMax(&doHistStretch,xMin);
		XDohiststretch_Start(&doHistStretch);
		// Do the DMA transfer to push and get our image
			axiTimer.startTimer();
			Xil_DCacheFlushRange((u32)imgIn_HW,SIZE_ARR*sizeof(unsigned char));
			Xil_DCacheFlushRange((u32)m_dma_buffer_RX,SIZE_ARR*sizeof(unsigned char));

			XAxiDma_SimpleTransfer(&axiDma,(u32)imgIn_HW,SIZE_ARR*sizeof(unsigned char),XAXIDMA_DMA_TO_DEVICE);
			XAxiDma_SimpleTransfer(&axiDma,(u32)m_dma_buffer_RX,SIZE_ARR*sizeof(unsigned char),XAXIDMA_DEVICE_TO_DMA);

			//Wait transfers to finish
			while(XAxiDma_Busy(&axiDma,XAXIDMA_DMA_TO_DEVICE));
			while(XAxiDma_Busy(&axiDma,XAXIDMA_DEVICE_TO_DMA));

			// Invalidate the cache to avoid reading garbage
			Xil_DCacheInvalidateRange((u32)m_dma_buffer_RX,SIZE_ARR*sizeof(unsigned char));
			axiTimer.stopTimer();

			double histstretch_HW_elapsed = axiTimer.getElapsedTimerInSeconds();
			printf(" hist stretch HW execution time: %f sec\n", histstretch_HW_elapsed);

double TotalSW = (hist_SW_elapsed+histstretch_SW_elapsed);
double TotalHW = (hist_HW_elapsed+histstretch_HW_elapsed);
printf("time summary SW%f HW%f ratio:%f\n ", TotalSW,TotalHW,TotalHW/TotalSW);
//dma out adree is m_buffer_Rx, can be used to do comparison between SW (imgOut) and HW output.
	return 0;
}
