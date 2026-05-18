#ifndef H_LIB_EXAMPLE_HW_H
#define H_LIB_EXAMPLE_HW_H

#include "xstatus.h"

// Define matrix dimensions (Must match your HLS synthesis parameters)
#define DIM    32
#define SIZE  ((DIM)*(DIM))

/**
 * Setup_HW_Accelerator
 * Initialises the HLS IP core using the Vitis 2025.2 driver API.
 */
int Setup_HW_Accelerator(float A[DIM][DIM], float B[DIM][DIM], float res_hw[DIM][DIM], int dma_size);

/**
 * Start_HW_Accelerator
 * Sets the 'ap_start' bit in the HLS IP control register.
 */
int Start_HW_Accelerator(void);

/**
 * Run_HW_Accelerator
 * Handles the DMA transfers:
 * 1. Flushes CPU cache for Input Matrices A and B.
 * 2. Initiates DMA Send for A and B.
 * 3. Initiates DMA Receive for the Result matrix.
 * 4. Invalidates CPU cache for Result matrix before software reads it.
 */
int Run_HW_Accelerator(float A[DIM][DIM], float B[DIM][DIM], float res_hw[DIM][DIM], int dma_size);

/**
 * matrix_multiply_ref
 * Software reference implementation for result verification.
 */
void matrix_multiply_ref(float a[DIM][DIM], float b[DIM][DIM], float out[DIM][DIM]);

#endif