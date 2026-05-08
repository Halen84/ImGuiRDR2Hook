#include "Manager.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC s_WndProc;
static bool s_bMouseInput = true;

static LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_XBUTTONDOWN: // Mouse back button
			s_bMouseInput ^= true;
			CImGuiMenu::SetShouldDrawMouse(s_bMouseInput == true);
			break;
		case WM_SYSKEYDOWN:
			if (wParam == VK_F10) {
				CImGuiMenu::SetIsOpen(!CImGuiMenu::GetIsOpen());
			}
			break;
		default: break;
	}

	if (s_bMouseInput)
	{
		if (CImGuiMenu::GetIsOpen()) {
			ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
			return true;
		}
	}
	else
	{
		// "Correct" way
		if (CImGuiMenu::GetIsOpen() && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) {
			return true;
		}
	}

	return CallWindowProc(s_WndProc, hwnd, uMsg, wParam, lParam);
}

void CImGuiHookManager::sWIN32::Hook()
{
	s_WndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (__int3264)(LONG_PTR)WndProc);
}

void CImGuiHookManager::sWIN32::Unhook()
{
	SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)s_WndProc);
}
