#include "Hook/Hook.h"

void DebugPrint(PCSTR text) {
#ifndef DEBUG
	UNREFERENCED_PARAMETER(text);
#endif // !DEBUG

	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, text));
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING regPath) {

	UNREFERENCED_PARAMETER(driverObject);
	UNREFERENCED_PARAMETER(regPath);

	Hook::CallKernelFunction(&Hook::HookHandler);

	return STATUS_SUCCESS;
}