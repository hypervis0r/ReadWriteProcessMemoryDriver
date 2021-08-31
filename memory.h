#pragma once

#include <ntifs.h>

#include "common.h"

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize);

NTSTATUS
RwMemReadProcessMemory(
	PIRP Irp,
	P_RWMEM_READ ReadInfo);

NTSTATUS
RwMemWriteProcessMemory(
	PIRP Irp,
	P_RWMEM_WRITE WriteInfo);