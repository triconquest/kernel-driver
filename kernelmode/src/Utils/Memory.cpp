#include "Memory.h"

PVOID Memory::GetSystemModuleBase(const char* moduleName) {

	ULONG bytes = 0;
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, NULL, bytes, &bytes);

	if (!bytes)
		return NULL;

	PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, 0x4e554c4c);

	if (modules == NULL)
		return NULL;

	status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);

	if (!NT_SUCCESS(status)) 
		return NULL;

	PRTL_PROCESS_MODULE_INFORMATION iModule = modules->Modules;
	PVOID moduleBase = 0, moduleSize = 0;

	for (ULONG i = 0; i < modules->NumberOfModules; i++) {

		if (strcmp((char*)iModule[i].FullPathName, moduleName) == 0) 
		{
			moduleBase = iModule[i].ImageBase;
			moduleSize = (PVOID)iModule[i].ImageSize;
			break;
		}
	}

	if (modules)
		ExFreePoolWithTag(modules, NULL);

	if (!moduleBase)
		return NULL;

	return moduleBase;
}

PVOID Memory::GetSystemModuleExport(const char* moduleName, LPCSTR routineName) {

	PVOID lpModule = GetSystemModuleBase(moduleName);

	if (!lpModule)
		return NULL;

	return RtlFindExportedRoutineByName(lpModule, routineName);
}

bool Memory::WriteMemory(void* address, void* buffer, size_t size)
{
	return RtlCopyMemory(address, buffer, size);
}

bool Memory::WriteToReadOnlyMemory(void* address, void* buffer, size_t size)
{
	PMDL mdl = IoAllocateMdl(address, size, FALSE, FALSE, NULL);

	if (!mdl)
		return false;

	MmProbeAndLockPages(mdl, KernelMode, IoReadAccess);
	PVOID mapping = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
	MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);

	WriteMemory(mapping, buffer, size);

	MmUnmapLockedPages(mapping, mdl);
	MmUnlockPages(mdl);
	IoFreeMdl(mdl);
	
	return true;
}
