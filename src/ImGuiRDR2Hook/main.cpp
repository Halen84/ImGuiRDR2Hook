#include "..\..\inc\main.h"
#include "script.h"
#include "keyboard.h"
#include "hooks/win32.h"
#include "hooks/dx12.h"
#include "hooks/vulkan.h"

#if !defined PROJECT_NAME
#define PROJECT_NAME "ImGuiRDR2Hook"
#endif


// Just to make sure some functions aren't called more than once
bool bVulkanInitialized = false;
bool bDX12Initialized = false;
bool bReadConfigFile = false;


void readConfigFile() noexcept
{
	std::ifstream file(PROJECT_NAME "_config.txt", std::ios::in);

	// File doesn't exist, default to use Vulkan API
	if (!file.good()) {
		bReadConfigFile = true;
		hooks::g_bUsingDX12Hook = false;
		hooks::g_bUsingVulkanHook = true;
		return;
	}

	std::string line = "";
	while (std::getline(file, line))
	{
		if (!line.empty() && !line.starts_with("//")) {
			size_t idx = line.find_first_of('=');
			std::string field = line.substr(0, idx);
			std::string value = line.substr(idx + 1);

			if (field == "bUseDirectX12") {
				if (value != "true") {
					hooks::g_bUsingDX12Hook = false;
				}
				else {
					hooks::g_bUsingDX12Hook = true;
				}
			}
			else if (field == "bUseVulkan") {
				if (value != "true") {
					hooks::g_bUsingVulkanHook = false;
				}
				else {
					hooks::g_bUsingVulkanHook = true;
				}
			}
		}
	}

	// Default to use Vulkan API
	if (hooks::g_bUsingDX12Hook == false && hooks::g_bUsingVulkanHook == false) {
		hooks::g_bUsingVulkanHook = true;
	}
	else if (hooks::g_bUsingDX12Hook == true && hooks::g_bUsingVulkanHook == true) {
		hooks::g_bUsingDX12Hook = false;
	}

	bReadConfigFile = true;
	file.close();
}


BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:

		if (!bReadConfigFile) {
			readConfigFile();
		}

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
		bReadConfigFile = false;

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
