#include "platform.h"
#include "xparameters.h"
#include "xaxidma.h"
#include "xhls_accel.h"
#include "lib_xmmult_hw.h"
#include "xil_printf.h"
#include "xil_cache.h"

// This manually satisfies the Linker's search for the hardware configuration
XHls_accel_Config XHls_accel_ConfigTable[] = {
	{
		0,                          // Device ID / Index
		XPAR_HLS_ACCEL_0_BASEADDR   // Base Address (Control_bus)
	},
	{0, 0}                          // Terminator
};

XHls_accel xmmult_dev;
extern XAxiDma AxiDma; // Defined in your main or globally

int XMmultSetup(){
    int status;
    XHls_accel_Config *ConfigPtr;

    // Modern Vitis Lookup
    ConfigPtr = XHls_accel_LookupConfig(XPAR_HLS_ACCEL_0_BASEADDR);
    if (!ConfigPtr) {
        xil_printf("Error: Could not find HLS Accel configuration\n");
        return XST_FAILURE;
    }

    status = XHls_accel_CfgInitialize(&xmmult_dev, ConfigPtr);
    if (status != XST_SUCCESS) {
        xil_printf("Error: HLS Accel Initialization failed\n");
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

void XMmultStart(void *InstancePtr){
    XHls_accel *pExample = (XHls_accel *)InstancePtr;
    XHls_accel_Start(pExample);
}

int Run_HW_Accelerator(float A[DIM][DIM], float B[DIM][DIM], float res_hw[DIM][DIM], int dma_size) {
    int status;

    // 1. Flush Cache (Push Matrix A and B from CPU -> DDR)
    Xil_DCacheFlushRange((UINTPTR)A, dma_size);
    Xil_DCacheFlushRange((UINTPTR)B, dma_size);
    // Note: No need to flush res_hw here, it's an output.

    // 2. START THE IP CORE FIRST
    // The IP must be "Ready" to receive the stream before the DMA starts pushing
    XMmultStart(&xmmult_dev);

    // 3. Transfer Matrix A (MM2S)
    status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)A, dma_size, XAXIDMA_DMA_TO_DEVICE);
    if (status != XST_SUCCESS) return XST_FAILURE;
    while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

    // 4. Transfer Matrix B (MM2S)
    status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)B, dma_size, XAXIDMA_DMA_TO_DEVICE);
    if (status != XST_SUCCESS) return XST_FAILURE;
    while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE));

    // 5. Setup Receive for Results (S2MM)
    status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR)res_hw, dma_size, XAXIDMA_DEVICE_TO_DMA);
    if (status != XST_SUCCESS) return XST_FAILURE;

    // 6. Wait for DMA to finish receiving from the IP
    while (XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA));
    
    // 7. Invalidate Cache (Pull Results from DDR -> CPU)
    // This forces the CPU to ignore its own cache and look at the new data in DDR
    Xil_DCacheInvalidateRange((UINTPTR)res_hw, dma_size);

    return XST_SUCCESS;
}

int Start_HW_Accelerator(void) {
    XMmultStart(&xmmult_dev);
    return XST_SUCCESS;
}

int Setup_HW_Accelerator(float A[DIM][DIM], float B[DIM][DIM], float res_hw[DIM][DIM], int dma_size) {
    
    return XMmultSetup();
}