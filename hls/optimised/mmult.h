#ifndef MMULT_H
#define MMULT_H

#include <assert.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>

#define MCR_SIZE 1024
typedef ap_axiu<32,4,5,5> AXI_VAL;

// Function prototypes
void HLS_accel (hls::stream<AXI_VAL>& INPUT_STREAM, hls::stream<AXI_VAL>& OUTPUT_STREAM);

/* ****************************** C++ TEMPLATES ************************************** */

// Reference function for verification
template <typename T, int DIM>
void matrix_multiply_ref(T a[DIM][DIM], T b[DIM][DIM], T out[DIM][DIM])
{
    for (int ia = 0; ia < DIM; ++ia) {
        for (int ib = 0; ib < DIM; ++ib) {
            float sum = 0;
            for (int id = 0; id < DIM; ++id) {
                sum += a[ia][id] * b[id][ib];
            }
            out[ia][ib] = sum;
        }
    }
}

// Hardware Core Logic (Optimized Version)
template <typename T, int DIM>
void mmult_hw(T a[DIM][DIM], T b[DIM][DIM], T out[DIM][DIM])
{
    int const FACTOR = DIM/16;
    // #pragma HLS INLINE
    #pragma HLS array_partition variable=a block factor=FACTOR dim=2
    #pragma HLS array_partition variable=b block factor=FACTOR dim=1

    L1:for (int ia = 0; ia < DIM; ++ia) {
		// #pragma HLS UNROLL off
        L2:for (int ib = 0; ib < DIM; ++ib) {
			// #pragma HLS UNROLL off
            #pragma HLS PIPELINE II=4
            T sum = 0;
            L3:for (int id = 0; id < DIM; ++id) {
				// #pragma HLS UNROLL factor=4
                sum += a[ia][id] * b[id][ib];
            }
            out[ia][ib] = sum;
        }
    }
}

// Pop stream: Extracts T (float) from AXI packet
template <typename T, int U, int TI, int TD>
T pop_stream(AXI_VAL const &e)
{
    #pragma HLS INLINE
    union {
        unsigned int ival;
        T oval;
    } converter;
    converter.ival = e.data.to_uint();
    return converter.oval;
}

// Push stream: Wraps T (float) into AXI packet
template <typename T, int U, int TI, int TD> 
AXI_VAL push_stream(T const &v, bool last = false)
{
    #pragma HLS INLINE
    AXI_VAL e;
    union {
        unsigned int oval;
        T ival;
    } converter;
    converter.ival = v;
    
    e.data = converter.oval;
    e.strb = -1;
    e.keep = 15; 
    e.user = 0;
    e.last = last ? 1 : 0;
    e.id = 0;
    e.dest = 0;
    return e;
}

// Wrapper for AXI-Stream interfaces
template <typename T, int DIM, int SIZE, int U, int TI, int TD>
void wrapped_mmult_hw (
    hls::stream<AXI_VAL>& in_stream,
    hls::stream<AXI_VAL>& out_stream)
{
    #pragma HLS INLINE

    T a[DIM][DIM];
    T b[DIM][DIM];
    T out[DIM][DIM];

    // Stream in Matrix A
    for(int i = 0; i < DIM; i++) {
        for(int j = 0; j < DIM; j++) {
            #pragma HLS PIPELINE II=1
            AXI_VAL tmp = in_stream.read(); // Read full packet
            a[i][j] = pop_stream<T,U,TI,TD>(tmp);
        }
    }

    // Stream in Matrix B
    for(int i = 0; i < DIM; i++) {
        for(int j = 0; j < DIM; j++) {
            #pragma HLS PIPELINE II=1
            AXI_VAL tmp = in_stream.read(); // Read full packet
            b[i][j] = pop_stream<T,U,TI,TD>(tmp);
        }
    }

    // HW Multiplication Core
    mmult_hw<T, DIM>(a, b, out);

    // Stream out Result Matrix
    for(int i = 0; i < DIM; i++) {
        for(int j = 0; j < DIM; j++) {
            #pragma HLS PIPELINE II=1
            bool is_last = (i == DIM-1 && j == DIM-1);
            out_stream.write(push_stream<T,U,TI,TD>(out[i][j], is_last));
        }
    }
}

#endif