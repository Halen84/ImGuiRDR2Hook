// Licensed under the MIT License - Halen84 (TuffyTown)

#include "script.h"
#include "keyboard.h"
#include "menu.h"
#include "hooks/vulkan.h"
#include "hooks/dx12.h"


void main()
{
	while (true)
	{
		// See hooks/vulkan.cpp --> RenderImGui_Vulkan() for drawing the mouse cursor and changing the style in Vulkan
		// See hooks/dx12.cpp --> hk_Present() for drawing the mouse cursor and changing the style in DirectX 12
		
		// ScriptHook's IsKey... functions don't work while a ImGui window is open (see #4 on GitHub)
		// If you want to change which key opens and closes the menu, also see menu.cpp
		if (IsKeyJustUp(VK_F5))
		{
			CMenu::bIsOpen ^= true;
		}

		WAIT(0);
	}
}


void ScriptMain()
{
	main();
}


#pragma warning(disable:28159)
void WaitAndRender(unsigned ms)
{
	using namespace hooks::vulkan;
	using namespace hooks::dx12;

	DWORD time = GetTickCount() + ms;
	bool waited = false;
	while (GetTickCount() < time || !waited)
	{
		WAIT(0);
		// This doesn't really work that well (see #2 on GitHub)
		if (hooks::bUsingVulkanHook)
		{
			hooks::vulkan::RenderImGui_Vulkan(m_queue, m_pPresentInfo);
		}
		else if (hooks::bUsingDX12Hook)
		{
			hooks::dx12::hk_Present(m_pSwapChain, m_SyncInterval, m_Flags);
		}
		waited = true;
	}
}
#pragma warning(default:28159)
