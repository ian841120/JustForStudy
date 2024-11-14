#pragma once
#include <windows.h>
#include <d3d11.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
class ImGuiClass
{
public:
	ImGuiClass(HWND hwnd,ID3D11Device* device,ID3D11DeviceContext* deviceContext);
	~ImGuiClass();
	void render();
	void newFrame();
	IMGUI_IMPL_API LRESULT handleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

};

//éQçlÅFhttps://github.com/ocornut/imgui/wiki/Getting-Started