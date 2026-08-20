#pragma once

#include <Windows.h>

class CProcess {
public:

	DWORD GetProcessID(const wchar_t* procName);
	uintptr_t GetModuleBase(const DWORD pID, const wchar_t* moduleName);

	DWORD processId;
	uintptr_t moduleBase;
};

inline CProcess Process;