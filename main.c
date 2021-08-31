#include "main.h"

NTSTATUS 
DriverIrpCreate(
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}

NTSTATUS
DriverIrpClose(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS
DriverIrpDeviceControl(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS ntResult = 0;
	PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);

	/*
		Execute IOCTL handler
	*/
	switch (Stack->Parameters.DeviceIoControl.IoControlCode)
	{
		case IOCTL_READ_PROCESS_MEMORY:
		{
			if (Stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(RWMEM_READ))
			{
				ntResult = STATUS_BUFFER_TOO_SMALL;
				break;
			}

			ntResult = RwMemReadProcessMemory(Irp, (P_RWMEM_READ)Stack->Parameters.DeviceIoControl.Type3InputBuffer);

			break;
		}
		case IOCTL_WRITE_PROCESS_MEMORY:
		{
			if (Stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(RWMEM_WRITE))
			{
				ntResult = STATUS_BUFFER_TOO_SMALL;
				break;
			}

			ntResult = RwMemWriteProcessMemory(Irp, (P_RWMEM_WRITE)Stack->Parameters.DeviceIoControl.Type3InputBuffer);
			
			break;
		}
		default:
		{
			ntResult = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}
	}

	/*
		Finish the I/O request with the final result of the IOCTL
	*/
	Irp->IoStatus.Status = ntResult;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return ntResult;
}

NTSTATUS 
DriverEntry(
	PDRIVER_OBJECT DriverObject, 
	PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS ntResult = 0;
	PDEVICE_OBJECT DeviceObject = NULL;
	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\RWMem");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\RWMem");

	/*
		Set our driver callbacks
	*/
	DriverObject->DriverUnload = DriverUnload;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverIrpCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverIrpClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverIrpDeviceControl;

	/*
		Create the device object
	*/
	ntResult = IoCreateDevice(
		DriverObject,
		0,
		&devName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject
	);
	if (!NT_SUCCESS(ntResult))
	{
		DbgPrint("[-] Failed to create symbolic link: 0x%08x\n", ntResult);

		return ntResult;
	}

	/*
		Create symlink to our kernel device object
	*/
	ntResult = IoCreateSymbolicLink(&symLink, &devName);
	if (!NT_SUCCESS(ntResult)) {
		DbgPrint("[-] Failed to create symbolic link: 0x%08X\n", ntResult);
		
		IoDeleteDevice(DeviceObject);
		return ntResult;
	}

	DbgPrint("[+] ReadWriteMemoryDriver loaded\n");

	return STATUS_SUCCESS;
}

void 
DriverUnload(
	PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\RWMem");

	IoDeleteSymbolicLink(&symLink);
	
	IoDeleteDevice(DriverObject->DeviceObject);

	DbgPrint("[+] ReadWriteMemoryDriver unloaded; bye :)\n");
}
