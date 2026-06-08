#include "Manager.h"
#include <vector>
#include <mutex>

bool CImGuiHookManager::m_initialized = false;
bool CImGuiHookManager::m_shutdownRequested = false;
bool CImGuiHookManager::m_drawMouse = true;
eHookGraphicsAPI CImGuiHookManager::m_hookAPI = eVULKAN;
HWND CImGuiHookManager::m_hWnd = NULL;

void CImGuiHookManager::RegisterImGuiMenuCB(RenderCallback cb, const char* id, int version)
{
	if (version != IMGUI_VERSION_NUM) {
		// Warn, but allow the version mismatch since there's a good chance the menu will still work fine
		char buf[256];
		sprintf_s(buf, "ImGui ID '%s' was built against ImGui %d, but host is using ImGui %d.\nThe menu may be unstable. Please download the correct ImGui headers.", 
			id ? id : "(null)", version, IMGUI_VERSION_NUM);
		MessageBoxA(nullptr, buf, "ImGui Host Version Mismatch", MB_OK | MB_ICONWARNING);
	}

	std::lock_guard<std::mutex> lock(m_callbackMutex);
	m_callbacks.emplace_back(cb, id);
}

void CImGuiHookManager::UnregisterImGuiMenuCB(RenderCallback cb)
{
	std::lock_guard<std::mutex> lock(m_callbackMutex);
	auto it = std::find_if(m_callbacks.begin(), m_callbacks.end(), 
		[cb] (const CallbackData& entry) {
			return entry.m_callback == cb;
		}
	);

	if (it != m_callbacks.end()) {
		m_callbacks.erase(it);
	}
}

void CImGuiHookManager::RunCallbacks()
{
	std::lock_guard<std::mutex> lock(CImGuiHookManager::m_callbackMutex);
	for (auto& cb : CImGuiHookManager::m_callbacks)
	{
		// TODO: probably a good idea to remove this from the array at this point
		if (cb.m_callback == nullptr) continue;

		if (cb.m_id == 0) {
			cb.m_callback();
		}
		else {
			ImGui::PushID(cb.m_id);
			cb.m_callback();
			ImGui::PopID();
		}
	}
}

void CImGuiHookManager::Initialize()
{
	if (m_initialized) return;

	// TODO: There is no way to actually let the user set the graphics API they want to use
	// since the hook is initialized in this .dll. I have it set so scripts will wait until
	// this .dll is loaded but by then the hook has already been initialized with the API set.
	// Instead, the user has to rely on this auto detection of the graphics API.
	// If vulkan-1.dll fails to load, it will automatically attempt to use DX12.
	//
	// I was not a fan of the way the previous API detection worked, and I am not sure if I like this or not.
	switch (m_hookAPI)
	{
		case eVULKAN: sVK::Hook(); break;
		case eDX12:   sDX12::Hook(); break;
		default: break;
	}

	m_initialized = true;
}

void CImGuiHookManager::Shutdown()
{
	if (!m_initialized) return;

	switch (m_hookAPI)
	{
		case eVULKAN: sVK::Unhook(); break;
		case eDX12:   sDX12::Unhook(); break;
		default: break;
	}

	sWIN32::Unhook();
	m_callbacks.clear();
	m_initialized = false;
}
