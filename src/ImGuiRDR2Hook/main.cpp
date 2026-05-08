#include "..\..\inc\main.h"
#include "script.h"
#include "keyboard.h"
#include "Hook/Manager.h"
#include <filesystem>

#if !defined PROJECT_NAME
#define PROJECT_NAME "ImGuiRDR2Hook"
#endif

//
// NOTICE:
// 
// - If you do not want to read from system.xml, and would rather read from the config, the set __READ_FROM_MY_CONFIG to TRUE
// - If you do not want to read from the config file, then set __FALLBACK_TO_MY_CONFIG to FALSE
// - If you do not want to read from either, set then set __FALLBACK_TO_MY_CONFIG to FALSE and set s_bHasConfigBeenRead to TRUE
//     - Be sure to set hooks::bUsingVulkanHook and hooks::bUsingDX12Hook accordingly
// 
// The config file is not required, but you will have to hardcode those booleans.
// If you do want to use it, then make sure it exists in your game directory.
//


// The config file is --> PROJECT_NAME "_config.txt"
// e.g. ImGuiRDR2Hook_config.txt
#define __FALLBACK_TO_MY_CONFIG 1
#define __READ_FROM_MY_CONFIG 0
#define __COMPILE_IMGUI 1


static bool s_bHasConfigBeenRead = false;
static void ReadMyConfigFile()
{
	if (!std::filesystem::exists("./" PROJECT_NAME "_config.txt"))
	{
		Log("[!] Config: " PROJECT_NAME "_config.txt does not exist. Using Vulkan API.");
		s_bHasConfigBeenRead = true;
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

			if (field == "HookType")
			{
				if (value == "Vulkan") {
					CImGuiHookManager::SetHookType(eVULKAN);
				}
				else if (value == "DX12") {
					CImGuiHookManager::SetHookType(eDX12);
				}
			}
		}
	}

	s_bHasConfigBeenRead = true;
	file.close();
}


static void ReadSystemXmlFile(bool bFallbackToMyConfig)
{
	char* userprofile;
	size_t length;
	errno_t didFail = _dupenv_s(&userprofile, &length, "USERPROFILE"); // std::getenv

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
						CImGuiHookManager::SetHookType(eVULKAN);
					}
					else if (strcmp(cstr, "    <API>kSettingAPI_DX12</API>") == 0)
					{
						CImGuiHookManager::SetHookType(eDX12);
					}

					s_bHasConfigBeenRead = true;
					break;
				}
			}
		}
		else
		{
			Log("[!] Config: Failed to find system.xml from %s. bFallbackToMyConfig: %d", settings.generic_string().c_str(), bFallbackToMyConfig);
			if (bFallbackToMyConfig)
			{
				ReadMyConfigFile();
			}
		}

		free(userprofile);
	}
	else
	{
		Log("[!] Config: _dupenv_s() failed. bFallbackToMyConfig: %d", bFallbackToMyConfig);
		if (bFallbackToMyConfig)
		{
			ReadMyConfigFile();
		}
	}
}


BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		if (!s_bHasConfigBeenRead)
		{
#if __COMPILE_IMGUI

			if (!s_bHasConfigBeenRead)
			{
				#if !__READ_FROM_MY_CONFIG
					ReadSystemXmlFile(__FALLBACK_TO_MY_CONFIG);
				#else
					ReadMyConfigFile();
				#endif //__READ_FROM_MY_CONFIG
			}
		}

		CImGuiHookManager::Initialize();
#endif //__COMPILE_IMGUI

		scriptRegister(hInstance, ScriptMain);
		keyboardHandlerRegister(OnKeyboardMessage);
		break;
	case DLL_PROCESS_DETACH:
#if __COMPILE_IMGUI
		s_bHasConfigBeenRead = false;
		CImGuiHookManager::Shutdown();

#endif //__COMPILE_IMGUI

		scriptUnregister(hInstance);
		keyboardHandlerUnregister(OnKeyboardMessage);
		break;
	}

	return TRUE;
}
