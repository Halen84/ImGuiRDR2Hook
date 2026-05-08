#pragma once
#include "script.h"
#include "imgui/imgui.h"

class CImGuiMenu
{
private:
	static bool sm_bMenuOpen;
	static bool sm_bDrawMouse;

public:
	static void Render();

	static inline bool GetIsOpen() { return sm_bMenuOpen; }
	static inline void SetIsOpen(bool open) { sm_bMenuOpen = open; }
	static inline bool ShouldDrawMouse() { return sm_bDrawMouse; }
	static inline void SetShouldDrawMouse(bool draw) { sm_bDrawMouse = draw; }
};
