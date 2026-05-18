/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xaxidma.h"
#include "xil_printf.h"
#include "lib_xmmult_hw.h" // Your updated hardware lib


// int main()
// {
//     init_platform();

//     print("Hello World\n\r");
//     print("Successfully ran Hello World application");
//     cleanup_platform();
//     return 0;
// }

// 1. Memory Allocation (The "Undeclared" variables)
float A[DIM][DIM];
float B[DIM][DIM];
float res_hw[DIM][DIM];

// 2. Hardware Instance
XAxiDma AxiDma;


int main() {
    // 1. Initialize the board hardware (UART, etc.)
    init_platform();

    xil_printf("--- Matrix Mult Test Starting ---\n\r");

    // 2. Initialize your Accelerator and DMA
    // (This calls the XMmultSetup we fixed earlier)
    int dma_size = SIZE * sizeof(float);
    int status = Setup_HW_Accelerator(A, B, res_hw, dma_size);
    
    if (status != XST_SUCCESS) {
        xil_printf("Hardware Setup Failed!\n\r");
        return -1;
    }

    // ... run your tests ...

    // 3. Cleanup (optional in most cases)
    cleanup_platform();
    return 0;
}