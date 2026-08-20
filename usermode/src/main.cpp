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

	CloseHandle(driver);

	std::cin.get();

	return 0;
}