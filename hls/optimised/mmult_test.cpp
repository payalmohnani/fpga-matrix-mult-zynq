#include <stdio.h>
#include <stdlib.h>
#include <hls_stream.h>

#include "mmult.h"

typedef float T;

const int DIM  = 32;
const int SIZE = DIM * DIM;

void mmult_sw(T a[DIM][DIM], T b[DIM][DIM], T out[DIM][DIM])
{
    for (int ia = 0; ia < DIM; ++ia)
        for (int ib = 0; ib < DIM; ++ib)
        {
            T sum = 0;

            for (int id = 0; id < DIM; ++id)
                sum += a[ia][id] * b[id][ib];

            out[ia][ib] = sum;
        }
}

template <typename T, int DIM, int SIZE, int U, int TI, int TD>
void standalone_mmult(T a[DIM][DIM],
                      T b[DIM][DIM],
                      T out[DIM][DIM])
{
    hls::stream<AXI_VAL> input_stream;
    hls::stream<AXI_VAL> output_stream;

    // Stream matrix A
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            input_stream.write(
                push_stream<T, U, TI, TD>(a[i][j], false)
            );
        }
    }

    // Stream matrix B
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            bool last = (i == DIM - 1) && (j == DIM - 1);

            input_stream.write(
                push_stream<T, U, TI, TD>(b[i][j], last)
            );
        }
    }

    // Run accelerator
    HLS_accel(input_stream, output_stream);

    // Read result matrix
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            AXI_VAL temp = output_stream.read();
            out[i][j] = pop_stream<T, U, TI, TD>(temp);
        }
    }
}

int main(void)
{
    int i, j, err;

    T matOp1[DIM][DIM];
    T matOp2[DIM][DIM];
    T matMult_sw_res[DIM][DIM];
    T matMult_hw[DIM][DIM];

    // Matrix initialisation
    for (i = 0; i < DIM; i++) {
        for (j = 0; j < DIM; j++) {
            matOp1[i][j] = (float)(i + j);
            matOp2[i][j] = (float)(i * j);
        }
    }

    printf("Running hardware accelerator...\n");

    standalone_mmult<T, DIM, SIZE, 4, 5, 5>(
        matOp1,
        matOp2,
        matMult_hw
    );

    printf("Running software reference...\n");

    mmult_sw(matOp1, matOp2, matMult_sw_res);

    // Compare results
    err = 0;

    for (i = 0; i < DIM; i++) {
        for (j = 0; j < DIM; j++) {
            if (matMult_sw_res[i][j] != matMult_hw[i][j]) {
                printf("Mismatch at [%d][%d]: SW = %f, HW = %f\n",
                       i, j,
                       matMult_sw_res[i][j],
                       matMult_hw[i][j]);
                err++;
            }
        }
    }

    if (err == 0)
        printf("Matrices identical ... Test successful!\n");
    else
        printf("Test failed! Total mismatches = %d\n", err);

    return err;
}