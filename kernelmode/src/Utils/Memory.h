#pragma once
#include "Definitions.h"

namespace Memory {
	PVOID GetSystemModuleBase(const char* moduleName);
	PVOID GetSystemModuleExport(const char* moduleName, LPCSTR routineName);

	bool WriteMemory(void* address, void* buffer, size_t size);
	bool WriteToReadOnlyMemory(void* address, void* buffer, size_t size);
}