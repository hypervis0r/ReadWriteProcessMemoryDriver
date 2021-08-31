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

	/*
		Get requesting process EPROCESS
	*/
	RequestingProcess = IoGetRequestorProcess(Irp);
	if (RequestingProcess == NULL)
	{
		DbgPrint("[-] Failed to find requestor process");
		return STATUS_ACCESS_DENIED;
	}

	/*
		Get target process EPROCESS
	*/
	ntResult = PsLookupProcessByProcessId((HANDLE)ReadInfo->ProcessId, &TargetProcess);
	if (!NT_SUCCESS(ntResult))
	{
		DbgPrint("[-] Failed to find process with PID: 0x%08x", ntResult);
		return ntResult;
	}

	DbgPrint("[+] Process %zu requested to read %zu bytes from process %zu at address %p\n",
		(ULONG_PTR)PsGetProcessId(RequestingProcess),
		ReadInfo->NumberOfBytesToRead,
		(ULONG_PTR)PsGetProcessId(TargetProcess),
		ReadInfo->AddressToRead);

	/*
		Copy memory from target process address to requesting process buffer
	*/
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

NTSTATUS
RwMemWriteProcessMemory(
	PIRP Irp,
	P_RWMEM_WRITE WriteInfo)
{
	NTSTATUS ntResult = 0;
	PEPROCESS TargetProcess = NULL;
	PEPROCESS RequestingProcess = NULL;

	if (!WriteInfo || !Irp)
		return STATUS_INVALID_PARAMETER;

	/*
		Get requesting process EPROCESS
	*/
	RequestingProcess = IoGetRequestorProcess(Irp);
	if (RequestingProcess == NULL)
	{
		DbgPrint("[-] Failed to find requestor process");
		return STATUS_ACCESS_DENIED;
	}

	/*
		Get target process EPROCESS
	*/
	ntResult = PsLookupProcessByProcessId((HANDLE)WriteInfo->ProcessId, &TargetProcess);
	if (!NT_SUCCESS(ntResult))
	{
		DbgPrint("[-] Failed to find process with PID: 0x%08x", ntResult);
		return ntResult;
	}

	DbgPrint("[+] Process %zu requested to write %zu bytes to process %zu at address %p\n", 
		(ULONG_PTR)PsGetProcessId(RequestingProcess),
		WriteInfo->NumberOfBytesToWrite,
		(ULONG_PTR)PsGetProcessId(TargetProcess),
		WriteInfo->AddressToWrite);

	/*
		Copy memory from requesting process buffer to target process address
	*/
	ntResult = MmCopyVirtualMemory(
		RequestingProcess,
		WriteInfo->Buffer,
		TargetProcess,
		WriteInfo->AddressToWrite,
		WriteInfo->NumberOfBytesToWrite,
		KernelMode,
		&WriteInfo->NumberOfBytesToWrite);
	if (!NT_SUCCESS(ntResult))
	{
		DbgPrint("[-] Failed to copy user buffer into process memory: 0x%08x", ntResult);
		return ntResult;
	}

	ObDereferenceObject(TargetProcess);

	return STATUS_SUCCESS;
}