// Licensed under the MIT License - Halen84 (TuffyTown)

#include "script.h"
#include "keyboard.h"
#include "menu.h"


void main()
{
	while (true)
	{
		// See hooks/vulkan.cpp --> RenderImGui_Vulkan() for drawing the mouse cursor and changing the style in Vulkan
		// See hooks/dx12.cpp --> hk_Present() for drawing the mouse cursor and changing the style in DirectX 12
		if (IsKeyJustUp(VK_F5)) {
			CMenu::bIsOpen ^= true;
		}

		WAIT(0);
	}
}


void ScriptMain()
{
	main();
}
