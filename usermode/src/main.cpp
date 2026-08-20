#include "Kernel/Driver.h"
#include "Utils/Process.h"

#include <iostream>

// usermode

int main() {
	printf("usermode2\n");

	const DWORD pID = Process.GetProcessID(L"Notepad.exe");

	if (pID == 0) {
		std::cout << "Failed to find notepad\n";
		std::cin.get();
		return 1;
	}

	const HANDLE driver = CreateFile(L"\\\\.\\2daydrv", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (driver == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to create our driver handle\n";
		std::cin.get();
		return 1;
	}

	if (Driver::AttachToProcess(driver, pID) == true) {
		std::cout << "Attachment successful\n";
	}

	const uintptr_t moduleBase = Process.GetModuleBase(pID, L"Notepad.exe");

	if (!moduleBase) {
		printf("moduleBase is NULL\n");
		return 1;
	}

	printf("[+] Notepad.exe moduleBase: 0x%llX\n", moduleBase);

	printf("[+] Doing a random read...");

	uintptr_t randomRead = Driver::Read<uintptr_t>(driver, moduleBase + 0x2F);

	printf("[+] Random read result: %llX\n", randomRead);

	printf("[+] Writing to a random address...\n");

	Driver::Write<int>(driver, moduleBase + 0x0B, 100);

	printf("[+] Wrote bs to some random address\n");


	CloseHandle(driver);

	std::cin.get();

	return 0;
}