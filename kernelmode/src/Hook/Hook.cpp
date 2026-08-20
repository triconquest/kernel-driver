#include "Hook.h"

bool Hook::CallKernelFunction(void* kernelFunctionAddress)
{
	if (!kernelFunctionAddress)
		return false;

	PVOID* function = reinterpret_cast<PVOID*>(Memory::GetSystemModuleExport("\\SystemRoot\\System32\\drivers\ (UNFINISHED)\", " < A KERNEL FUNCTION WHIHC NEVER GETS CALLED > "");

	if (!function)
		return false;

	BYTE orig[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	BYTE shellCode[] = { 0x48, 0xB8 };
	BYTE shellCodeEnd[] = { 0xFF, 0xE0 };

	RtlSecureZeroMemory(&orig, sizeof(orig));
	memcpy((PVOID)((ULONG_PTR)orig), &shellCode, sizeof(shellCode));
	uintptr_t hookAddress = reinterpret_cast<uintptr_t>(kernelFunctionAddress);
	memcpy((PVOID)((ULONG_PTR)orig + sizeof(shellCode)), &hookAddress, sizeof(void*));
	memcpy((PVOID)((ULONG_PTR)orig + sizeof(shellCode) + sizeof(void*)), &shellCodeEnd, sizeof(shellCodeEnd));

	Memory::WriteToReadOnlyMemory(function, &orig, sizeof(orig));

	return true;
}

NTSTATUS Hook::HookHandler(PVOID calledParam)
{
	return STATUS_SUCCESS;
}
