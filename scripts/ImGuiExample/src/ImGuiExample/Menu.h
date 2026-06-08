#pragma once
#include "imgui/imgui.h"

class CImGuiMenu
{
private:
	static inline bool m_bIsOpen = true;
public:
	static void Render();
};
