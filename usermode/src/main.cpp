#include "Kernel/Driver.h"
#include "Utils/Process.h"
#include "Features/ESP.h"
#include "Menu/Menu.h"

#include <iostream>

// usermode

int main() {

	const wchar_t* processName = L"Notepad.exe";
	const wchar_t* moduleName = L"Notepad.exe"; //L"r5apex_dx12.exe"  

	const DWORD pID = Process.GetProcessID(processName);

	if (pID == 0) {
		std::cout << "Failed to find process\n";
		std::cin.get();
		return 1;
	}

	printf("[+] Process found. PID: %lu\n", pID);

	const HANDLE driver = CreateFile(L"\\\\.\\2daydrv", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (driver == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to create our driver handle\n";
		std::cin.get();
		return 1;
	}

	if (!Driver::AttachToProcess(driver, pID)) {
		std::cout << "Failed to attach to process\n";
		CloseHandle(driver);
		std::cin.get();
		return 1;
	}

	std::cout << "Attachment successful\n";

	ESP.SetDriverHandle(driver);

	uintptr_t moduleBase = 0;
	if (!Driver::GetModuleBase(driver, pID, moduleBase)) {
		printf("[!] Coulnd't get module base\n");
		CloseHandle(driver);
		std::cin.get();
		return 1;
	}

	if (!moduleBase) {
		printf("[!] moduleBase is NULL\n");
		CloseHandle(driver);
		std::cin.get();
		return 1;
	}

	printf("[+] moduleBase: 0x%llX\n", moduleBase);

	// overlay init
	OverlayWindow overlay;
	if (!overlay.Initialize()) {
		printf("[!] Menu failed to initialize\n");
		CloseHandle(driver);
		std::cin.get();
		return -1;
	}

	printf("[+] Menu initialized\n");

	while (!overlay.ShouldClose() && !(GetAsyncKeyState(VK_HOME) & 0x8000)) {

		if (GetAsyncKeyState(VK_INSERT) & 1)
			Vars::bMenuOpen = !Vars::bMenuOpen;

		overlay.BeginFrame();
		overlay.RenderMenu();

		if (Vars::ESP::bEnabled)
			ESP.Run(moduleBase);

		overlay.EndFrame();
	}

	printf("[+] Cleaning...\n");
	CloseHandle(driver);
	overlay.Cleanup();
	std::cin.get();

	return 0;
}
