#include "Manager.h"

bool CImGuiHookManager::m_initialized = false;
bool CImGuiHookManager::m_shutdownRequested = false;
HookType CImGuiHookManager::m_hookType = eVULKAN;
HWND CImGuiHookManager::m_hWnd = NULL;
CImGuiHookManager::sVK CImGuiHookManager::m_vulkanData{};
CImGuiHookManager::sDX12 CImGuiHookManager::m_dx12Data{};
CImGuiHookManager::sWIN32 CImGuiHookManager::m_win32Data{};


void CImGuiHookManager::Initialize()
{
	if (m_initialized) return;

	switch (m_hookType)
	{
		case eVULKAN: m_vulkanData.Hook(); break;
		case eDX12:   m_dx12Data.Hook(); break;
		default: break;
	}

	m_initialized = true;
}

void CImGuiHookManager::Shutdown()
{
	if (!m_initialized) return;

	switch (m_hookType)
	{
		case eVULKAN: m_vulkanData.Unhook(); break;
		case eDX12:   m_dx12Data.Unhook(); break;
		default: break;
	}

	m_win32Data.Unhook();
	m_initialized = false;
}
