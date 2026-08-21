#include "Kernel/Driver.h"
#include "Utils/Process.h"
#include "Menu/Menu.h"

#include <iostream>

// usermode

int main() {

	const wchar_t* moduleName = L"r5apex_dx12.exe";
	const DWORD pID = Process.GetProcessID(L"Notepad.exe"); // L"r5apex_dx12.exe"

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

	if (Driver::AttachToProcess(driver, pID) == true) {
		std::cout << "Attachment successful\n";
	}

	const uintptr_t moduleBase = Process.GetModuleBase(pID, L"Notepad.exe");

	if (!moduleBase) {
		printf("[!] moduleBase is NULL\n");
		std::cin.get();
		return 1;
		
	}

	printf("[+] moduleBase: 0x%llX\n", moduleBase);

	// overlay init
	OverlayWindow overlay;
	if (!overlay.Initialize()) {
		printf("[!] Menu failed to initialize\n");
		return -1;
	}

	printf("[+] Menu initialized\n");

	if (GetAsyncKeyState(VK_HOME)) {

		if (GetAsyncKeyState(VK_INSERT) & 1)
			Vars::bMenuOpen = !Vars::bMenuOpen;

		overlay.BeginFrame();
		overlay.RenderMenu();

		ImDrawList* drawList = ImGui::GetBackgroundDrawList();

		// call here

		
		overlay.EndFrame();
	}

	printf("[+] Cleaning...\n");
	CloseHandle(driver);
	overlay.Cleanup();
	std::cin.get();

	return 0;
}