#pragma once

#include <ntifs.h>

#include "common.h"
#include "memory.h"

NTSTATUS
DriverIrpCreate(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp);

NTSTATUS
DriverIrpClose(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp);

NTSTATUS
DriverEntry(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath);

void
DriverUnload(
	PDRIVER_OBJECT DriverObject);