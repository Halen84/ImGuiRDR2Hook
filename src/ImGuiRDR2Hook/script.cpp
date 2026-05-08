#include "script.h"
#include "keyboard.h"
#include "menu.h"
#include "Hook/Manager.h"

void main()
{
	while (true)
	{
		// ...

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
	DWORD time = GetTickCount() + ms;
	bool waited = false;
	while (GetTickCount() < time || !waited)
	{
		WAIT(0);

		// This doesn't really work that well (see #2 on GitHub)
		switch (CImGuiHookManager::GetHookType())
		{
			case eVULKAN:
				CImGuiHookManager::GetVulkan().Present();
				break;
			case eDX12:
				CImGuiHookManager::GetDX12().Present();
				break;
			default: break;
		}

		waited = true;
	}
}
#pragma warning(default:28159)
