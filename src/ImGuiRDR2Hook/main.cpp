#include "Hook/Manager.h"
#include <filesystem>

static bool s_bHasConfigBeenRead = false;

static void ReadSystemXmlFile()
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
				if (line.find("kSettingAPI_Vulkan") != std::string::npos)
				{
					CImGuiHookManager::SetGraphicsAPI(eVULKAN);
					break;
				}
				else if (line.find("kSettingAPI_DX12") != std::string::npos)
				{
					CImGuiHookManager::SetGraphicsAPI(eDX12);
					break;
				}
			}
		}
		else
		{
			Log("[!] Config: Failed to find system.xml at %s", settings.generic_string().c_str());
		}

		free(userprofile);
	}
	else {
		Log("[!] Config: _dupenv_s() failed with error code %d", didFail);
	}
}


BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		// Commenting out for now
		//if (!s_bHasConfigBeenRead)
		//{
		//	ReadSystemXmlFile();
		//	s_bHasConfigBeenRead = true;
		//}

		// After some Windows (security?) update in May 2026 (?), something changed that causes the game to fail to load.
		// Using DebugView sysinternals I was seeing an error like "Cannot call CreateDXGIFactory from DllMain".
		// Yielding until the game window is found before doing any hooking seems to do the trick.
		// Presumably this is what I should've been doing in the first place.
		DisableThreadLibraryCalls(hInstance);
		CloseHandle(CreateThread(nullptr, 0,(LPTHREAD_START_ROUTINE)[](LPVOID) -> DWORD
		{
			constexpr ULONGLONG TIMEOUT_MS = 15 * 1000; // 15 seconds
			const ULONGLONG start = GetTickCount64();

			while (FindWindowA(NULL, "Red Dead Redemption 2") == NULL)
			{
				// Timeout failsafe in case for whatever reason something goes wrong
				if (GetTickCount64() - start >= TIMEOUT_MS) {
					return 0;
				}

				Sleep(100);
			}

			CImGuiHookManager::Initialize();

			return 0;
		},
		nullptr, 0, nullptr));
		break;
	case DLL_PROCESS_DETACH:
		s_bHasConfigBeenRead = false;
		CImGuiHookManager::Shutdown();
		break;
	}

	return TRUE;
}
