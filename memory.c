#include "memory.h"

NTSTATUS
RwMemReadProcessMemory(
	PIRP Irp,
	P_RWMEM_READ ReadInfo)
{
	NTSTATUS ntResult = 0;
	PEPROCESS TargetProcess = NULL;
	PEPROCESS RequestingProcess = NULL;

	if (!ReadInfo || !Irp)
		return STATUS_INVALID_PARAMETER;

	RequestingProcess = IoGetRequestorProcess(Irp);
	if (RequestingProcess == NULL)
	{
		DbgPrint("[-] Failed to find requestor process");
		return STATUS_ACCESS_DENIED;
	}

	ntResult = PsLookupProcessByProcessId((HANDLE)ReadInfo->ProcessId, &TargetProcess);
	if (!NT_SUCCESS(ntResult))
	{
		DbgPrint("[-] Failed to find process with PID: 0x%08x", ntResult);
		return ntResult;
	}

	ntResult = MmCopyVirtualMemory(
		TargetProcess, 
		ReadInfo->AddressToRead, 
		RequestingProcess, 
		ReadInfo->Buffer, 
		ReadInfo->NumberOfBytesToRead, 
		KernelMode, 
		&ReadInfo->NumberOfBytesToRead);
	if (!NT_SUCCESS(ntResult))
	{
		DbgPrint("[-] Failed to copy process memory into user buffer: 0x%08x", ntResult);
		return ntResult;
	}

	ObDereferenceObject(TargetProcess);

	return STATUS_SUCCESS;
}