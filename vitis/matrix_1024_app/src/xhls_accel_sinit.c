// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2025.2 (64-bit)
// Tool Version Limit: 2025.11
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#ifdef SDT
#include "xparameters.h"
#endif
#include "xhls_accel.h"

extern XHls_accel_Config XHls_accel_ConfigTable[];

#ifdef SDT
XHls_accel_Config *XHls_accel_LookupConfig(UINTPTR BaseAddress) {
	XHls_accel_Config *ConfigPtr = NULL;

	int Index;

	for (Index = (u32)0x0; XHls_accel_ConfigTable[Index].Name != NULL; Index++) {
		if (!BaseAddress || XHls_accel_ConfigTable[Index].Control_bus_BaseAddress == BaseAddress) {
			ConfigPtr = &XHls_accel_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XHls_accel_Initialize(XHls_accel *InstancePtr, UINTPTR BaseAddress) {
	XHls_accel_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XHls_accel_LookupConfig(BaseAddress);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XHls_accel_CfgInitialize(InstancePtr, ConfigPtr);
}
#else
XHls_accel_Config *XHls_accel_LookupConfig(u16 DeviceId) {
	XHls_accel_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XHLS_ACCEL_NUM_INSTANCES; Index++) {
		if (XHls_accel_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XHls_accel_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XHls_accel_Initialize(XHls_accel *InstancePtr, u16 DeviceId) {
	XHls_accel_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XHls_accel_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XHls_accel_CfgInitialize(InstancePtr, ConfigPtr);
}
#endif

#endif

