#pragma once
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "../Utils/Vars.h"

#include <Windows.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <cmath>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

inline LRESULT CALLBACK OverlayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

class OverlayWindow {
private:
	HWND windowHandle;
	WNDCLASSEXW windowClass;

	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderTarget;

	bool clickThrough;
	bool shouldClose;

	bool SetupD3D11(HWND hwnd) {
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
		D3D_FEATURE_LEVEL obtainedLevel;

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
			levels, 2, D3D11_SDK_VERSION, &sd,
			&swapChain, &d3dDevice, &obtainedLevel, &d3dContext
		);

		if (hr == DXGI_ERROR_UNSUPPORTED) {
			hr = D3D11CreateDeviceAndSwapChain(
				nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0,
				levels, 2, D3D11_SDK_VERSION, &sd,
				&swapChain, &d3dDevice, &obtainedLevel, &d3dContext
			);
		}

		if (FAILED(hr))
			return false;

		ID3D11Texture2D* backBuffer = nullptr;
		if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) || backBuffer == nullptr)
			return false;

		hr = d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &renderTarget);
		backBuffer->Release();

		return SUCCEEDED(hr);
	}

	void CleanupD3D11() {
		if (renderTarget) { renderTarget->Release(); renderTarget = nullptr; }
		if (swapChain) { swapChain->Release();    swapChain = nullptr; }
		if (d3dContext) { d3dContext->Release();   d3dContext = nullptr; }
		if (d3dDevice) { d3dDevice->Release();    d3dDevice = nullptr; }
	}

	void ApplyModernTheme() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding = 10.0f;
		style.ChildRounding = 8.0f;
		style.FrameRounding = 6.0f;
		style.PopupRounding = 6.0f;
		style.ScrollbarRounding = 6.0f;
		style.GrabRounding = 4.0f;
		style.TabRounding = 6.0f;

		style.WindowPadding = ImVec2(14.0f, 14.0f);
		style.FramePadding = ImVec2(10.0f, 6.0f);
		style.ItemSpacing = ImVec2(10.0f, 8.0f);
		style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
		style.ScrollbarSize = 6.0f;
		style.GrabMinSize = 10.0f;
		style.WindowBorderSize = 0.0f;
		style.ChildBorderSize = 0.0f;
		style.FrameBorderSize = 0.0f;

		ImVec4* c = style.Colors;

		c[ImGuiCol_WindowBg] = ImVec4(0.051f, 0.059f, 0.078f, 1.00f); // #0d0f14
		c[ImGuiCol_ChildBg] = ImVec4(0.075f, 0.086f, 0.118f, 1.00f); // #13161e
		c[ImGuiCol_PopupBg] = ImVec4(0.102f, 0.118f, 0.157f, 1.00f); // #1a1e28

		c[ImGuiCol_Border] = ImVec4(0.165f, 0.188f, 0.271f, 1.00f); // #2a3045
		c[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

		c[ImGuiCol_FrameBg] = ImVec4(0.122f, 0.141f, 0.196f, 1.00f); // #1f2433
		c[ImGuiCol_FrameBgHovered] = ImVec4(0.165f, 0.188f, 0.271f, 1.00f);
		c[ImGuiCol_FrameBgActive] = ImVec4(0.196f, 0.220f, 0.318f, 1.00f);

		c[ImGuiCol_TitleBg] = ImVec4(0.051f, 0.059f, 0.078f, 1.00f);
		c[ImGuiCol_TitleBgActive] = ImVec4(0.051f, 0.059f, 0.078f, 1.00f);
		c[ImGuiCol_TitleBgCollapsed] = ImVec4(0.051f, 0.059f, 0.078f, 1.00f);

		c[ImGuiCol_ScrollbarBg] = ImVec4(0.075f, 0.086f, 0.118f, 1.00f);
		c[ImGuiCol_ScrollbarGrab] = ImVec4(0.165f, 0.188f, 0.271f, 1.00f);
		c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.302f, 0.486f, 0.996f, 1.00f); // accent
		c[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.227f, 0.373f, 0.839f, 1.00f);

		c[ImGuiCol_CheckMark] = ImVec4(0.302f, 0.486f, 0.996f, 1.00f);

		c[ImGuiCol_SliderGrab] = ImVec4(0.302f, 0.486f, 0.996f, 1.00f);
		c[ImGuiCol_SliderGrabActive] = ImVec4(0.227f, 0.373f, 0.839f, 1.00f);

		c[ImGuiCol_Button] = ImVec4(0.122f, 0.141f, 0.196f, 1.00f);
		c[ImGuiCol_ButtonHovered] = ImVec4(0.302f, 0.486f, 0.996f, 0.20f);
		c[ImGuiCol_ButtonActive] = ImVec4(0.302f, 0.486f, 0.996f, 0.35f);

		c[ImGuiCol_Header] = ImVec4(0.302f, 0.486f, 0.996f, 0.18f);
		c[ImGuiCol_HeaderHovered] = ImVec4(0.302f, 0.486f, 0.996f, 0.28f);
		c[ImGuiCol_HeaderActive] = ImVec4(0.302f, 0.486f, 0.996f, 0.38f);

		c[ImGuiCol_Separator] = ImVec4(0.165f, 0.188f, 0.271f, 1.00f);
		c[ImGuiCol_SeparatorHovered] = ImVec4(0.302f, 0.486f, 0.996f, 0.50f);
		c[ImGuiCol_SeparatorActive] = ImVec4(0.302f, 0.486f, 0.996f, 1.00f);

		c[ImGuiCol_ResizeGrip] = ImVec4(0.302f, 0.486f, 0.996f, 0.15f);
		c[ImGuiCol_ResizeGripHovered] = ImVec4(0.302f, 0.486f, 0.996f, 0.40f);
		c[ImGuiCol_ResizeGripActive] = ImVec4(0.302f, 0.486f, 0.996f, 0.70f);

		c[ImGuiCol_Text] = ImVec4(0.886f, 0.910f, 0.941f, 1.00f); // #e2e8f0
		c[ImGuiCol_TextDisabled] = ImVec4(0.420f, 0.463f, 0.600f, 1.00f); // muted
	}

public:
	OverlayWindow() : windowHandle(nullptr), d3dDevice(nullptr), d3dContext(nullptr),
		swapChain(nullptr), renderTarget(nullptr), clickThrough(true), shouldClose(false) {
		ZeroMemory(&windowClass, sizeof(windowClass));
	}

	static void PushRainbowText(float offset = 0.0f, float speed = 0.4f, float sat = 0.85f, float val = 1.0f) {
		float hue = fmodf((float)ImGui::GetTime() * speed + offset, 1.0f);
		ImVec4 color;
		ImGui::ColorConvertHSVtoRGB(hue, sat, val, color.x, color.y, color.z);
		color.w = 1.0f;
		ImGui::PushStyleColor(ImGuiCol_Text, color);
	}

	bool Initialize() {
		windowClass.cbSize = sizeof(WNDCLASSEXW);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = OverlayWndProc;
		windowClass.hInstance = GetModuleHandleW(nullptr);
		windowClass.lpszClassName = L"dead rising";

		if (!RegisterClassExW(&windowClass))
			return false;

		int screenW = GetSystemMetrics(SM_CXSCREEN);
		int screenH = GetSystemMetrics(SM_CYSCREEN);

		windowHandle = CreateWindowExW(
			WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
			windowClass.lpszClassName, L"dead rising",
			WS_POPUP, 0, 0, screenW, screenH,
			nullptr, nullptr, windowClass.hInstance, nullptr
		);

		if (!windowHandle)
			return false;

		SetLayeredWindowAttributes(windowHandle, RGB(0, 0, 0), 255, LWA_ALPHA);

		MARGINS margins = { -1, -1, -1, -1 };
		DwmExtendFrameIntoClientArea(windowHandle, &margins);

		ShowWindow(windowHandle, SW_SHOW);
		UpdateWindow(windowHandle);

		if (!SetupD3D11(windowHandle)) {
			CleanupD3D11();
			return false;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;

		ApplyModernTheme();

		ImGui_ImplWin32_Init(windowHandle);
		ImGui_ImplDX11_Init(d3dDevice, d3dContext);

		return true;
	}

	void BeginFrame() {
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				shouldClose = true;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		const bool wantClickThrough = !Vars::bMenuOpen;

		if (wantClickThrough != clickThrough) {
			LONG exStyle = WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW;

			if (wantClickThrough)
				exStyle |= WS_EX_TRANSPARENT;

			SetWindowLong(windowHandle, GWL_EXSTYLE, exStyle);
			clickThrough = wantClickThrough;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void RenderMenu() {
		if (!Vars::bMenuOpen) return;

		ImGui::SetNextWindowSize(ImVec2(650, 450), ImGuiCond_FirstUseEver);
		ImGui::Begin("dead rising", &Vars::bMenuOpen,
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

		ImGui::BeginChild("TabBar", ImVec2(150, 0), false);

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
		//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.420f, 0.463f, 0.600f, 1.0f));
		PushRainbowText(0.0f);
		ImGui::Text("  MENU");
		ImGui::PopStyleColor();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.0f);

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.165f, 0.188f, 0.271f, 1.0f));
		ImGui::Separator();
		ImGui::PopStyleColor();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8.0f);

		ImVec2 buttonSize(130, 36);

		if (Vars::currentTab == 0) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.302f, 0.486f, 0.996f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.361f, 0.533f, 1.000f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.227f, 0.373f, 0.839f, 1.00f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.00f));
			if (ImGui::Button("General", buttonSize)) Vars::currentTab = 0;
			ImGui::PopStyleColor(4);
		}
		else {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.302f, 0.486f, 0.996f, 0.12f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.302f, 0.486f, 0.996f, 0.22f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.420f, 0.463f, 0.600f, 1.00f));
			if (ImGui::Button("General", buttonSize)) Vars::currentTab = 0;
			ImGui::PopStyleColor(4);
		}

		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("Content", ImVec2(0, 0), false);

		if (Vars::currentTab == 0) {

			ImGui::Checkbox("Aimbot", &Vars::Aimbot::bEnabled);
			ImGui::Checkbox("ESP", &Vars::ESP::bEnabled);
		}

		ImGui::EndChild();
		ImGui::End();
	}

	void EndFrame() {
		ImGui::Render();

		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		d3dContext->OMSetRenderTargets(1, &renderTarget, nullptr);
		d3dContext->ClearRenderTargetView(renderTarget, clearColor);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		swapChain->Present(1, 0);
	}

	void Cleanup() {
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupD3D11();

		if (windowHandle) {
			DestroyWindow(windowHandle);
			windowHandle = nullptr;
		}

		UnregisterClassW(windowClass.lpszClassName, windowClass.hInstance);
	}

	HWND GetWindowHandle() const { return windowHandle; }

	bool ShouldClose() const { return shouldClose; }
};