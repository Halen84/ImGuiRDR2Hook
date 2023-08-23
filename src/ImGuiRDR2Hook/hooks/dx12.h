#pragma once
#include "include.h"
#include <dxgi1_4.h>

namespace hooks {
namespace dx12 {

	// DOT NOT USE THIS VARIABLE - ONLY TO BE USED WITH WaitAndRender()
	inline IDXGISwapChain3* m_pSwapChain;
	// DOT NOT USE THIS VARIABLE - ONLY TO BE USED WITH WaitAndRender()
	inline UINT m_SyncInterval;
	// DOT NOT USE THIS VARIABLE - ONLY TO BE USED WITH WaitAndRender()
	inline UINT m_Flags;
	// DOT NOT USE THIS FUNCTION - ONLY TO BE USED WITH WaitAndRender()
	long __fastcall hk_Present(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags);

	void Hook();
	void Unhook();

} // namespace dx12
} // namespace hooks
