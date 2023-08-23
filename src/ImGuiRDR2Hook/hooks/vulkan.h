#pragma once
#include "include.h"

namespace hooks {
namespace vulkan {
	// DOT NOT USE THIS VARIABLE - ONLY TO BE USED WITH WaitAndRender()
	inline VkQueue m_queue;
	// DOT NOT USE THIS VARIABLE - ONLY TO BE USED WITH WaitAndRender()
	inline const VkPresentInfoKHR* m_pPresentInfo;
	// DOT NOT USE THIS FUNCTION - ONLY TO BE USED WITH WaitAndRender()
	void RenderImGui_Vulkan(VkQueue queue, const VkPresentInfoKHR* pPresentInfo);

	void Hook();
	void Unhook();

} // namespace vulkan
} // namespace hooks
