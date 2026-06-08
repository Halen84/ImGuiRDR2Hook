#pragma once
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>
#include <dxgi1_4.h>
#include "MinHook.h"
#include "../kiero/kiero.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"
#include "../imgui/imgui_impl_vulkan.h"

#define _LOGGING_ENABLED 0

enum eHookGraphicsAPI : char
{
	eVULKAN,
	eDX12,
};

class __declspec(dllexport) CImGuiHookManager
{
	using RenderCallback = void(*)();

public:
	struct sVK
	{
		static void Present();
		static void Hook();
		static void Unhook();
	};

	struct sDX12
	{
		static void Present();
		static void Hook();
		static void Unhook();
	};

	struct sWIN32
	{
		static void Hook();
		static void Unhook();
	};

	static void RegisterImGuiMenuCB(RenderCallback cb, const char* id = 0, int version = IMGUI_VERSION_NUM);
	static void UnregisterImGuiMenuCB(RenderCallback cb);
	static void RunCallbacks();

private:
	static bool m_initialized;
	static bool m_shutdownRequested;
	static bool m_drawMouse;
	static eHookGraphicsAPI m_hookAPI;
	static HWND m_hWnd;

	struct CallbackData
	{
		RenderCallback m_callback = nullptr;
		const char* m_id = 0;
	};

	static inline std::vector<CallbackData> m_callbacks;
	static inline std::mutex m_callbackMutex;
public:
	static void Initialize();
	static void Shutdown();

	static bool IsInitialized() { return m_initialized; }
	static bool IsShutdownRequested() { return m_shutdownRequested; }
	static eHookGraphicsAPI GetGraphicsAPI() { return m_hookAPI; }
	static void SetGraphicsAPI(eHookGraphicsAPI api = eVULKAN) { m_hookAPI = api; }
	static HWND GetGameWindow() { return m_hWnd; }
	static void SetGameWindow(HWND hWnd) { m_hWnd = hWnd; }
	static bool ShouldDrawMouse() { return m_drawMouse; }
	static void SetShouldDrawMouse(bool draw) { m_drawMouse = draw; }
};


namespace hooks
{	
	inline const char* KieroStatusEnumToString(kiero::Status::Enum status)
	{
		switch (status)
		{
			case kiero::Status::Success:					return "Success";
			case kiero::Status::UnknownError:				return "UnknownError";
			case kiero::Status::NotSupportedError:			return "NotSupportedError";
			case kiero::Status::ModuleNotFoundError:		return "ModuleNotFoundError";
			case kiero::Status::AlreadyInitializedError:	return "AlreadyInitializedError";
			case kiero::Status::NotInitializedError:		return "NotInitializedError";
		}

		return "";
	}
}

inline void Log(const char* format, ...)
{
#if _LOGGING_ENABLED
	std::ofstream file(PROJECT_NAME ".log", std::ios::app);
	std::ostringstream msg;
	
	va_list args;
	va_start(args, format);
	char buf[1024];
	vsnprintf(buf, sizeof(buf), format, args);
	va_end(args);

	msg << buf << '\n';
	file << msg.str();
#endif
}
