// Licensed under the MIT License - Halen84 (TuffyTown)

#pragma once
#include "script.h"
#include "imgui/imgui.h"

inline void DisableAllControlActionsThisFrame()
{
	int controlIndex = PAD::IS_USING_KEYBOARD_AND_MOUSE(0) ? 0 : 2;
	PAD::DISABLE_ALL_CONTROL_ACTIONS(controlIndex);
}

class CMenu
{
public:
	static bool bIsOpen;

public:
	static void Draw();
};
