#include "..\..\inc\main.h"
#include "script.h"
#include "keyboard.h"

#include "hooks/win32.h"
#include "hooks/dx12.h"
#include "hooks/vulkan.h"

#ifdef VULKAN_HOOK
#pragma message("Compiling the project with Vulkan...")
#elif DX12_HOOK
#pragma message("Compiling the project with DirectX 12...")
#endif

// Just in case. Make sure Hook() isn't called more than once
bool bVulkanInitialized = false;
bool bDX12Initialized = false;

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:

		if (!bVulkanInitialized && hooks::g_bUsingVulkanHook) {
			hooks::vulkan::Hook();
			bVulkanInitialized = true;
		}

		if (!bDX12Initialized && hooks::g_bUsingDX12Hook) {
			hooks::dx12::Hook();
			bVulkanInitialized = true;
		}

		// hooks::win32::Hook() is automatically called

		scriptRegister(hInstance, ScriptMain);
		keyboardHandlerRegister(OnKeyboardMessage);
		break;
	case DLL_PROCESS_DETACH:
		bVulkanInitialized = false;
		bDX12Initialized = false;

		if (hooks::g_bUsingDX12Hook) {
			hooks::dx12::Unhook();
		}
		else if (hooks::g_bUsingVulkanHook) {
			hooks::vulkan::Unhook();
		}

		// hooks::win32::Unhook() is automatically called

		scriptUnregister(hInstance);
		keyboardHandlerUnregister(OnKeyboardMessage);
		break;
	}

	return TRUE;
}
