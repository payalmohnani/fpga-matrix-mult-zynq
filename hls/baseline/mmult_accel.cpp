#include <stdio.h>
#include <stdlib.h>
#include <hls_stream.h>
#include "mmult.h"

// THIS IS THE TOP LEVEL DESIGN
// Standardized for Vitis HLS 2025.2

void HLS_accel (hls::stream<AXI_VAL>& INPUT_STREAM, hls::stream<AXI_VAL>& OUTPUT_STREAM)
{
    // Interface pragmas for AXI-Stream and AXI-Lite Control
    #pragma HLS INTERFACE s_axilite port=return bundle=CONTROL_BUS
    #pragma HLS INTERFACE axis port=OUTPUT_STREAM
    #pragma HLS INTERFACE axis port=INPUT_STREAM

    // Execute the multiplication on 32x32 tiles
    // 1024 is the total number of elements per matrix (32*32)
    wrapped_mmult_hw <float, 32, 1024, 4, 5, 5>(INPUT_STREAM, OUTPUT_STREAM);

    return;
}

// Optional: Keep for non-synthesized tests
void standalone_mmult (float A[32][32], float B[32][32], float C[32][32])
{
    mmult_hw <float, 32>(A, B, C);
}