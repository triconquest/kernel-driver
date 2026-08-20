#include "Process.h"

#include <TlHelp32.h>
#include <iostream>

DWORD CProcess::GetProcessID(const wchar_t* procName)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(snapshot, &entry))
	{
		do {
			if (!wcscmp(procName, entry.szExeFile))
			{
				CloseHandle(snapshot);
				this->processId = entry.th32ProcessID; 
				return entry.th32ProcessID;
			}
		} while (Process32Next(snapshot, &entry));
	}

	printf("[!] Process doesn't exist\n");
	CloseHandle(snapshot);
	return 0;
}

uintptr_t CProcess::GetModuleBase(const DWORD pID, const wchar_t* moduleName)
{
	MODULEENTRY32 entry;
	entry.dwSize = sizeof(MODULEENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pID);

	if (Module32First(snapshot, &entry))
	{
		do {
			if (wcsstr(moduleName, entry.szModule) != nullptr)
			{
				CloseHandle(snapshot);
				this->moduleBase = (uintptr_t)entry.modBaseAddr;
				return (uintptr_t)entry.modBaseAddr;
			}
		} while (Module32Next(snapshot, &entry));
	}

	printf("[!] Couldn't get module\n");
	CloseHandle(snapshot);
	return 0;
}


