# FPGA-Accelerated Matrix Multiplication on Zynq-7000

Tiled FPGA-accelerated floating-point matrix multiplication system implemented on the Zybo Z7 (XC7Z010) using Vitis HLS, Vivado, AXI4-Stream, and AXI DMA.

## Overview

This project implements a hardware/software co-designed floating-point matrix multiplication accelerator on the AMD/Xilinx Zynq-7000 SoC platform.

Due to FPGA resource limitations, a tiled computation architecture was used to support large 1024×1024 matrix multiplication using a reusable 32×32 hardware accelerator.

The ARM Cortex-A9 Processing System (PS) manages:

* DMA configuration
* DDR3 memory management
* Tile scheduling
* Result accumulation

The Programmable Logic (PL) hosts:

* Custom HLS matrix multiplication accelerator
* AXI DMA
* AXI interconnect infrastructure

---

## Hardware Platform

* Board: Zybo Z7-10
* FPGA: XC7Z010-1CLG400C
* SoC: Zynq-7000
* Clock Frequency: 100 MHz

---

## Tools Used

* Vitis HLS 2025.2
* Vivado 2025.2
* Vitis Unified IDE 2025.2

---

## System Architecture

### Processing System (PS)

* ARM Cortex-A9
* DDR3 memory controller
* AXI control configuration
* DMA orchestration

### Programmable Logic (PL)

* Floating-point matrix multiplication accelerator
* AXI DMA
* AXI4-Stream interfaces
* AXI4-Lite control interface

### Communication Interfaces

| Interface   | Purpose                         |
| ----------- | ------------------------------- |
| AXI4-Lite   | Control and status registers    |
| AXI4-Stream | High-speed matrix data transfer |
| AXI DMA     | DDR ↔ Accelerator data movement |
| HP0 Port    | High-bandwidth DDR access       |

---

## Design Flow

1. Matrix multiplication kernel implemented in C++
2. HLS optimisation using:

   * Loop pipelining
   * Array partitioning
3. RTL generation using Vitis HLS
4. Vivado block design integration
5. AXI DMA integration
6. Bitstream generation
7. Software application development in Vitis
8. Hardware validation on Zybo Z7 board

---

## HLS Optimisation Stages

### Solution 1 — Sequential Baseline

* No pipelining
* No unrolling
* Latency: 329,793 cycles

### Solution 2 — Pipeline + Auto-Unroll

* Loop pipelining enabled
* Memory bottleneck observed
* Latency: 16,535 cycles

### Solution 3 — Fully Optimised

* Loop pipelining
* Array partitioning
* II = 4 achieved
* Latency: 8,351 cycles

---

## Performance Results

| Metric    | Baseline       | Optimised    |
| --------- | -------------- | ------------ |
| Latency   | 329,793 cycles | 8,351 cycles |
| Speed-up  | 1×             | 39.5×        |
| DSP Usage | 5              | 20           |
| LUT Usage | 935            | 5,492        |

---

## Tiled Computation Strategy

A full 1024×1024 matrix multiplication cannot fit entirely within FPGA BRAM resources.

To overcome this:

* Matrices are partitioned into 32×32 tiles
* Tiles are streamed into the accelerator using AXI DMA
* Partial results are accumulated in DDR3 memory
* The same accelerator hardware is reused iteratively

This enables scalable large-matrix computation without increasing FPGA resource usage.

---

## Verification

The design was validated using:

* C Simulation
* C/RTL Co-Simulation
* Vivado waveform analysis

Verification included:

* AXI4-Stream handshake validation
* Pipeline scheduling analysis
* DMA transfer validation
* Floating-point correctness checks

---

## Repository Structure

```text id="j1hyy6"
fpga-matrix-mult-zynq/
│
├── hls/
├── vivado/
├── vitis/
├── reports/
├── docs/
├── images/
└── README.md
```

---

## Key Features

* Floating-point matrix multiplication accelerator
* AXI DMA-based PS–PL communication
* AXI4-Stream dataflow architecture
* Hardware/software co-design
* Tiled matrix computation
* DDR3-backed large matrix processing
* HLS optimisation exploration
* Resource/performance tradeoff analysis


