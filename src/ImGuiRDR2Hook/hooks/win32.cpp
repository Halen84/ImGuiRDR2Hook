#include "win32.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC	og_WndProc;


LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (CMenu::bIsOpen) {
		ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		return true;
	}

	return CallWindowProc(og_WndProc, hwnd, uMsg, wParam, lParam);
}


namespace hooks {
namespace win32 {
	
	void Hook()
	{
		og_WndProc = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
	}

	void Unhook()
	{
		SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)og_WndProc);
	}

} // namespace win32
} // namespace hooks

