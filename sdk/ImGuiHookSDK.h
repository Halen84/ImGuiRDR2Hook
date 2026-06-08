#pragma once
#include "imgui/imgui.h"

class __declspec(dllimport) CImGuiHookManager
{
	using RenderCallback = void(*)();

public:

	// Registers a ImGui menu callback to be called every frame.
	//
	// Note: 'id' is an optional unique identifier for your menu. It is used as an
	// ImGui ID scope (PushID/PopID) around your menu callback, preventing widget ID conflicts with
	// other scripts. It DOES NOT resolve window name conflicts! To avoid those, append '##your_unique_id_here'
	// to any ImGui::Begin() calls in your menu.
	static void RegisterImGuiMenuCB(RenderCallback cb, const char* id = 0, int version = IMGUI_VERSION_NUM);

	// Unregisters a previously registered ImGui menu callback. Make sure to call this before your script is unloaded for memory performance
	static void UnregisterImGuiMenuCB(RenderCallback cb);
};
