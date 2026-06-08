#include "..\..\inc\main.h"
#include "script.h"
#include "keyboard.h"
#include "Menu.h"
#include "ImGuiHookSDK.h"

BOOL APIENTRY DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		
		// We cannot guarantee that ImGuiRDR2Hook.dll will be loaded before other scripts,
		// so we need to wait for it to be loaded before doing anything.
		CloseHandle(CreateThread(nullptr, 0, [](LPVOID) -> DWORD
		{
			while (GetModuleHandleA("ImGuiRDR2Hook.dll") == NULL) {
				Sleep(100);
			}

			CImGuiHookManager::RegisterImGuiMenuCB(&CImGuiMenu::Render, "example_uid");

			return 0;
		}, nullptr, 0, nullptr));

		scriptRegister(hinstDLL, main);
		keyboardHandlerRegister(OnKeyboardMessage);
		break;
	case DLL_PROCESS_DETACH:
		CImGuiHookManager::UnregisterImGuiMenuCB(&CImGuiMenu::Render); // Don't forget to unregister!
		scriptUnregister(hinstDLL);
		keyboardHandlerUnregister(OnKeyboardMessage);
		break;
	}

	return TRUE;
}
