#include "Utils/Definitions.h"

void DebugPrint(PCSTR text) {
#ifndef DEBUG
	UNREFERENCED_PARAMETER(text);
#endif // !DEBUG

	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
}

NTSTATUS driver_main(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath) {
	UNREFERENCED_PARAMETER(registryPath);

	UNICODE_STRING deviceName = {};
	RtlInitUnicodeString(&deviceName, L"\\Driver\\2daydrv");

	PDEVICE_OBJECT deviceObject = nullptr;
	NTSTATUS status = IoCreateDevice(driverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject);

	if (status != STATUS_SUCCESS) {
		DebugPrint("[!] Failed to create driver device\n");
		return status;
	}

	DebugPrint("[+] Driver device successfully created\n");

	UNICODE_STRING symbolicLink = {};
	RtlInitUnicodeString(&symbolicLink, L"\\DosDevices\\2daydrv");

	status = IoCreateSymbolicLink(&symbolicLink, &deviceName);

	if (status != STATUS_SUCCESS) {
		DebugPrint("[!] Failed to establish symoblic link\n");
		return status;
	}

	DebugPrint("[+] Driver link successfully created\n");

	SetFlag(deviceObject->Flags, DO_BUFFERED_IO);

	driverObject->MajorFunction[IRP_MJ_CREATE] = Driver::Create;
	driverObject->MajorFunction[IRP_MJ_CLOSE] = Driver::Close;
	driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = Driver::DeviceControl;

	ClearFlag(deviceObject->Flags, DO_DEVICE_INITIALIZING);

	DebugPrint("[+] Driver initialized\n");

	return status;
}

extern "C" NTSTATUS DriverEntry() {

	DebugPrint("in the kernel\n");

	UNICODE_STRING driverName = {};
	RtlInitUnicodeString(&driverName, L"\\Driver\\2daydrv");

	return IoCreateDriver(&driverName, &driver_main);
}