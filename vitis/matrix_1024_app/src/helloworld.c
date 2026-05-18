#include <stdio.h>
#include <string.h> // For memcpy
#include "platform.h"
#include "xaxidma.h"
#include "xil_printf.h"
#include "lib_xmmult_hw.h" 

// Dimensions
#define FULL_DIM 1024
#define TILE_DIM 32 // Matches your HLS IP DIM
#define BYTES_PER_TILE (TILE_DIM * TILE_DIM * sizeof(float))

// 1. Large Matrices in DDR
// Declared as static global to stay off the stack (Avoids crash)
static float A_large[FULL_DIM][FULL_DIM] __attribute__ ((aligned(32)));
static float B_large[FULL_DIM][FULL_DIM] __attribute__ ((aligned(32)));
static float C_large[FULL_DIM][FULL_DIM] __attribute__ ((aligned(32)));

// 2. Local buffers for the 32x32 Hardware IP
// These must be used for the DMA transfers
static float A_tile[TILE_DIM][TILE_DIM] __attribute__ ((aligned(32)));
static float B_tile[TILE_DIM][TILE_DIM] __attribute__ ((aligned(32)));
static float res_tile[TILE_DIM][TILE_DIM] __attribute__ ((aligned(32)));

// Hardware Instance
XAxiDma AxiDma;

int main() {
    init_platform();
    xil_printf("\r\n--- 1024x1024 Tiled Matrix Mult Starting ---\r\n");

    // 1. Setup Accelerator (using the tile buffers for initialization)
    int status = Setup_HW_Accelerator(A_tile, B_tile, res_tile, BYTES_PER_TILE);
    if (status != XST_SUCCESS) {
        xil_printf("Hardware Setup Failed!\r\n");
        return -1;
    }

    // 2. Initialize large matrices with dummy data
    for(int i = 0; i < FULL_DIM; i++) {
        for(int j = 0; j < FULL_DIM; j++) {
            A_large[i][j] = 1.0f; 
            B_large[i][j] = 2.0f;
            C_large[i][j] = 0.0f;
        }
    }

    // 3. Tiled Matrix Multiplication
    for (int i = 0; i < FULL_DIM; i += TILE_DIM) {
        for (int j = 0; j < FULL_DIM; j += TILE_DIM) {
            
            // Clear result block in large matrix before accumulation
            for(int r = 0; r < TILE_DIM; r++)
                for(int c = 0; c < TILE_DIM; c++) 
                    C_large[i + r][j + c] = 0;

            for (int k = 0; k < FULL_DIM; k += TILE_DIM) {
                
                // --- Step A: Packing Tiles ---
                for (int row = 0; row < TILE_DIM; row++) {
                    // memcpy is fast for A because rows are contiguous
                    memcpy(A_tile[row], &A_large[i + row][k], TILE_DIM * sizeof(float));
                    // Manual loop for B because we are jumping through columns in memory
                    for (int col = 0; col < TILE_DIM; col++) {
                        B_tile[row][col] = B_large[k + row][j + col];
                    }
                }

                // --- Step B: Run Hardware for 32x32 Tile ---
                // This includes Flush -> Start -> DMA -> Invalidate
                Run_HW_Accelerator(A_tile, B_tile, res_tile, BYTES_PER_TILE);

                // --- Step C: Software Accumulation ---
                for (int row = 0; row < TILE_DIM; row++) {
                    for (int col = 0; col < TILE_DIM; col++) {
                        C_large[i + row][j + col] += res_tile[row][col];
                    }
                }
            }
        }
        // Progress update every 128 rows so you know it's not dead
        if (i % 128 == 0) xil_printf("Completed rows through %d...\r\n", i + TILE_DIM);
    }

    xil_printf("Computation Finished!\r\n");

    // 4. Verification: For 1.0 * 2.0 over 1024 elements, result should be 2048.0
    printf("Verification: C[0][0] = %f (Expected: 2048.0)\r\n", (double)C_large[0][0]);

    cleanup_platform();
    return 0;
}