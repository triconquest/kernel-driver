#pragma once

#include <Windows.h>

class CProcess {
public:

	DWORD GetProcessID(const wchar_t* procName);
	uintptr_t GetModuleBase(const DWORD pID, const wchar_t* moduleName);

	DWORD processId = 0;
	uintptr_t moduleBase = 0;
};

inline CProcess Process;