// Licensed under the MIT License - Halen84 (TuffyTown)

#include "..\..\inc\main.h"
#include "script.h"
#include "keyboard.h"
#include "hooks/win32.h"
#include "hooks/dx12.h"
#include "hooks/vulkan.h"
#include <filesystem>

#if !defined PROJECT_NAME
#define PROJECT_NAME "ImGuiRDR2Hook"
#endif

//
// NOTICE:
// 
// - If you do not want to read from system.xml, and would rather read from our config, the set __READ_FROM_OUR_CONFIG to TRUE
// - If you do not want to read from our config file, then set __FALLBACK_TO_OUR_CONFIG to FALSE
// - If you do not want to read from either, set then set __FALLBACK_TO_OUR_CONFIG to FALSE and set bHasConfigBeenRead to TRUE
//     - Be sure to set hooks::bUsingVulkanHook and hooks::bUsingDX12Hook accordingly
// 
// The config file is not required, but you will have to hardcode those booleans.
// If you do want to use it, then make sure it exists in your game directory.
//


// Our config file is --> PROJECT_NAME "_config.txt"
// e.g. ImGuiRDR2Hook_config.txt
#define __FALLBACK_TO_OUR_CONFIG 1
#define __READ_FROM_OUR_CONFIG 0


// Just to make sure some functions aren't called more than once
bool bVulkanInitialized = false;
bool bDX12Initialized = false;
bool bHasConfigBeenRead = false;


void ReadOurConfigFile() noexcept
{
	// Default to use Vulkan API (these will be changed if this function passes)
	hooks::bUsingVulkanHook = true;
	hooks::bUsingDX12Hook = false;

	// File doesn't exist, abort
	if (!std::filesystem::exists("./" PROJECT_NAME "_config.txt"))
	{
		Log("[!] Config: " PROJECT_NAME "_config.txt does not exist. Using Vulkan API.");
		bHasConfigBeenRead = true;
		return;
	}

	std::ifstream file(PROJECT_NAME "_config.txt", std::ios::in);
	std::string line = "";
	while (std::getline(file, line))
	{
		if (!line.empty() && !line.starts_with("//"))
		{
			size_t idx = line.find_first_of('=');
			std::string field = line.substr(0, idx);
			std::string value = line.substr(idx + 1);

			if (field == "bUseDirectX12")
			{
				if (value != "true")
				{
					hooks::bUsingDX12Hook = false;
				}
				else
				{
					hooks::bUsingDX12Hook = true;
				}
			}
			else if (field == "bUseVulkan")
			{
				if (value != "true")
				{
					hooks::bUsingVulkanHook = false;
				}
				else
				{
					hooks::bUsingVulkanHook = true;
				}
			}
		}
	}

	// Default to use Vulkan API
	if (hooks::bUsingDX12Hook == false && hooks::bUsingVulkanHook == false)
	{
		hooks::bUsingVulkanHook = true;
		Log("[!] Config: bUsingDX12Hook & bUsingVulkanHook were both FALSE. Using Vulkan API.");
	}
	else if (hooks::bUsingDX12Hook == true && hooks::bUsingVulkanHook == true)
	{
		hooks::bUsingDX12Hook = false;
		Log("[!] Config: bUsingDX12Hook & bUsingVulkanHook were both TRUE. Using Vulkan API.");
	}

	bHasConfigBeenRead = true;
	file.close();
}


void ReadSystemXmlFile(bool bFallbackToOurConfig) noexcept
{
	char* userprofile;
	size_t length;
	errno_t didFail = _dupenv_s(&userprofile, &length, "USERPROFILE"); // std::getenv

	// Default to use Vulkan API
	hooks::bUsingVulkanHook = true;
	hooks::bUsingDX12Hook = false;

	if (!didFail)
	{
		const auto settings = std::filesystem::path(userprofile) / "Documents\\Rockstar Games\\Red Dead Redemption 2\\Settings\\system.xml";	
		if (std::filesystem::exists(settings))
		{
			std::ifstream stream(settings.generic_string().c_str());
			std::string line;

			while (std::getline(stream, line))
			{
				// fuck it, we will deal with the whitespace
				if (line.starts_with("    <API>"))
				{
					const char* cstr = line.c_str();

					if (strcmp(cstr, "    <API>kSettingAPI_Vulkan</API>") == 0)
					{
						hooks::bUsingVulkanHook = true;
						hooks::bUsingDX12Hook = false;
					}
					else if (strcmp(cstr, "    <API>kSettingAPI_DX12</API>") == 0)
					{
						hooks::bUsingVulkanHook = false;
						hooks::bUsingDX12Hook = true;
					}

					bHasConfigBeenRead = true;
					break;
				}
			}
		}
		else
		{
			Log("[!] Config: Failed to find system.xml from %s. bFallbackToOurConfig: %d", settings.generic_string().c_str(), bFallbackToOurConfig);
			if (bFallbackToOurConfig)
			{
				ReadOurConfigFile();
			}
		}

		free(userprofile);
	}
	else
	{
		Log("[!] Config: _dupenv_s() failed. bFallbackToOurConfig: %d", bFallbackToOurConfig);
		if (bFallbackToOurConfig)
		{
			ReadOurConfigFile();
		}
	}
}


BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		if (!bHasConfigBeenRead)
		{
#if !__READ_FROM_OUR_CONFIG
			ReadSystemXmlFile(__FALLBACK_TO_OUR_CONFIG);
#else
			ReadOurConfigFile();
#endif
		}

		if (!bVulkanInitialized && hooks::bUsingVulkanHook)
		{
			hooks::vulkan::Hook();
			bVulkanInitialized = true;
		}

		if (!bDX12Initialized && hooks::bUsingDX12Hook)
		{
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
		bHasConfigBeenRead = false;

		if (hooks::bUsingDX12Hook)
		{
			hooks::dx12::Unhook();
		}
		else if (hooks::bUsingVulkanHook)
		{
			hooks::vulkan::Unhook();
		}

		// hooks::win32::Unhook() is automatically called

		scriptUnregister(hInstance);
		keyboardHandlerUnregister(OnKeyboardMessage);
		break;
	}

	return TRUE;
}
