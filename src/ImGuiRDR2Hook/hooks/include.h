// Licensed under the MIT License - Halen84 (TuffyTown)

#pragma once
#include <Windows.h>
#include <fstream>
#include <sstream>
#include "MinHook.h"
#include "../menu.h"
#include "../kiero/kiero.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_win32.h"
#include "../imgui/imgui_impl_dx12.h"
#include "../imgui/imgui_impl_vulkan.h"


// Set whether the hooks should log debug information
#define _LOGGING_ENABLED 0


namespace hooks {
	inline bool g_bUsingDX12Hook = false;
	inline bool g_bUsingVulkanHook = true;

	inline HWND g_hWnd = NULL;
	inline bool g_bShutdownRequested = false;
	inline bool g_bImGuiInitialized = false;
	
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

	inline const char* MHStatusToString(MH_STATUS status)
	{
		switch (status)
		{
			case MH_UNKNOWN:					return "Unknown MinHook error";
			case MH_OK:							return "Successful";
			case MH_ERROR_ALREADY_INITIALIZED:	return "MinHook is already initialized.";
			case MH_ERROR_NOT_INITIALIZED:		return "MinHook is not initialized yet: or already uninitialized.";
			case MH_ERROR_ALREADY_CREATED:		return "The hook for the specified target function is already created.";
			case MH_ERROR_NOT_CREATED:			return "The hook for the specified target function is not created yet.";
			case MH_ERROR_ENABLED:				return "The hook for the specified target function is already enabled.";
			case MH_ERROR_DISABLED:				return "The hook for the specified target function is not enabled yet: or already disabled.";
			case MH_ERROR_NOT_EXECUTABLE:		return "The specified pointer is invalid. It points the address of non-allocated and/or non-executable region.";
			case MH_ERROR_UNSUPPORTED_FUNCTION: return "The specified target function cannot be hooked.";
			case MH_ERROR_MEMORY_ALLOC:			return "Failed to allocate memory.";
			case MH_ERROR_MEMORY_PROTECT:		return "Failed to change the memory protection.";
			case MH_ERROR_MODULE_NOT_FOUND:		return "The specified module is not loaded.";
			case MH_ERROR_FUNCTION_NOT_FOUND:	return "The specified function is not found.";
		}

		return "";
	}
}

inline void Log(const char* format, ...)
{
#if _LOGGING_ENABLED
	std::ofstream file("ImGuiRDR2Hook.log", std::ios::app);
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
