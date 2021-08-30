#pragma once

#include <ntifs.h>

#define RWMEM_DEVICE 0x8000
#define IOCTL_READ_PROCESS_MEMORY CTL_CODE(RWMEM_DEVICE, 0x800, METHOD_NEITHER, FILE_ANY_ACCESS)

typedef struct RWMEM_READ
{
	UINT32 ProcessId;
	PVOID AddressToRead;
	SIZE_T NumberOfBytesToRead;
	PVOID Buffer;
} RWMEM_READ, * P_RWMEM_READ;