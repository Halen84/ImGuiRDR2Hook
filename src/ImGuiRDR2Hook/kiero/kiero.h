#ifndef __KIERO_H__
#define __KIERO_H__

#include <stdint.h>

#define KIERO_VERSION "1.2.12"

#define KIERO_INCLUDE_D3D9   0 // 1 if you need D3D9 hook
#define KIERO_INCLUDE_D3D10  0 // 1 if you need D3D10 hook
#define KIERO_INCLUDE_D3D11  0 // 1 if you need D3D11 hook
#define KIERO_INCLUDE_D3D12  DX12_HOOK // 1 if you need D3D12 hook
#define KIERO_INCLUDE_OPENGL 0 // 1 if you need OpenGL hook
#define KIERO_INCLUDE_VULKAN VULKAN_HOOK // 1 if you need Vulkan hook
#define KIERO_USE_MINHOOK    1 // 1 if you will use kiero::bind function

#define KIERO_ARCH_X64 0
#define KIERO_ARCH_X86 0

#if defined(_M_X64)	
# undef  KIERO_ARCH_X64
# define KIERO_ARCH_X64 1
#else
# undef  KIERO_ARCH_X86
# define KIERO_ARCH_X86 1
#endif

#if KIERO_ARCH_X64
typedef uint64_t uint150_t;
#else
typedef uint32_t uint150_t;
#endif

namespace kiero
{
	struct Status
	{
		enum Enum
		{
			UnknownError = -1,
			NotSupportedError = -2,
			ModuleNotFoundError = -3,

			AlreadyInitializedError = -4,
			NotInitializedError = -5,

			Success = 0,
		};
	};

	struct RenderType
	{
		enum Enum
		{
			None,

			D3D9,
			D3D10,
			D3D11,
			D3D12,

			OpenGL,
			Vulkan,

			Auto
		};
	};

	Status::Enum init(RenderType::Enum renderType);
	void shutdown();

	Status::Enum bind(uint16_t index, void** original, void* function);
	void unbind(uint16_t index);

	RenderType::Enum getRenderType();
	uint150_t* getMethodsTable();

	struct D3D12MT
	{
		enum Enum : int
		{
			ExecuteCommandLists = 54,
			Present = 140,
		};
	};

	struct VULKANMT
	{
		enum Enum : int
		{
			vkCreateInstance = 0,
			vkDestroyInstance = 1,
			vkEnumeratePhysicalDevices = 2,
			vkGetPhysicalDeviceFeatures = 3,
			vkGetPhysicalDeviceFormatProperties = 4,
			vkGetPhysicalDeviceImageFormatProperties = 5,
			vkGetPhysicalDeviceProperties = 6,
			vkGetPhysicalDeviceQueueFamilyProperties = 7,
			vkGetPhysicalDeviceMemoryProperties = 8,
			vkGetInstanceProcAddr = 9,
			vkGetDeviceProcAddr = 10,
			vkCreateDevice = 11,
			vkDestroyDevice = 12,
			vkEnumerateInstanceExtensionProperties = 13,
			vkEnumerateDeviceExtensionProperties = 14,
			vkEnumerateDeviceLayerProperties = 15,
			vkGetDeviceQueue = 16,
			vkQueueSubmit = 17,
			vkQueueWaitIdle = 18,
			vkDeviceWaitIdle = 19,
			vkAllocateMemory = 20,
			vkFreeMemory = 21,
			vkMapMemory = 22,
			vkUnmapMemory = 23,
			vkFlushMappedMemoryRanges = 24,
			vkInvalidateMappedMemoryRanges = 25,
			vkGetDeviceMemoryCommitment = 26,
			vkBindBufferMemory = 27,
			vkBindImageMemory = 28,
			vkGetBufferMemoryRequirements = 29,
			vkGetImageMemoryRequirements = 30,
			vkGetImageSparseMemoryRequirements = 31,
			vkGetPhysicalDeviceSparseImageFormatProperties = 32,
			vkQueueBindSparse = 33,
			vkCreateFence = 34,
			vkDestroyFence = 35,
			vkResetFences = 36,
			vkGetFenceStatus = 37,
			vkWaitForFences = 38,
			vkCreateSemaphore = 39,
			vkDestroySemaphore = 40,
			vkCreateEvent = 41,
			vkDestroyEvent = 42,
			vkGetEventStatus = 43,
			vkSetEvent = 44,
			vkResetEvent = 45,
			vkCreateQueryPool = 46,
			vkDestroyQueryPool = 47,
			vkGetQueryPoolResults = 48,
			vkCreateBuffer = 49,
			vkDestroyBuffer = 50,
			vkCreateBufferView = 51,
			vkDestroyBufferView = 52,
			vkCreateImage = 53,
			vkDestroyImage = 54,
			vkGetImageSubresourceLayout = 55,
			vkCreateImageView = 56,
			vkDestroyImageView = 57,
			vkCreateShaderModule = 58,
			vkDestroyShaderModule = 59,
			vkCreatePipelineCache = 60,
			vkDestroyPipelineCache = 61,
			vkGetPipelineCacheData = 62,
			vkMergePipelineCaches = 63,
			vkCreateGraphicsPipelines = 64,
			vkCreateComputePipelines = 65,
			vkDestroyPipeline = 66,
			vkCreatePipelineLayout = 67,
			vkDestroyPipelineLayout = 68,
			vkCreateSampler = 69,
			vkDestroySampler = 70,
			vkCreateDescriptorSetLayout = 71,
			vkDestroyDescriptorSetLayout = 72,
			vkCreateDescriptorPool = 73,
			vkDestroyDescriptorPool = 74,
			vkResetDescriptorPool = 75,
			vkAllocateDescriptorSets = 76,
			vkFreeDescriptorSets = 77,
			vkUpdateDescriptorSets = 78,
			vkCreateFramebuffer = 79,
			vkDestroyFramebuffer = 80,
			vkCreateRenderPass = 81,
			vkDestroyRenderPass = 82,
			vkGetRenderAreaGranularity = 83,
			vkCreateCommandPool = 84,
			vkDestroyCommandPool = 85,
			vkResetCommandPool = 86,
			vkAllocateCommandBuffers = 87,
			vkFreeCommandBuffers = 88,
			vkBeginCommandBuffer = 89,
			vkEndCommandBuffer = 90,
			vkResetCommandBuffer = 91,
			vkCmdBindPipeline = 92,
			vkCmdSetViewport = 93,
			vkCmdSetScissor = 94,
			vkCmdSetLineWidth = 95,
			vkCmdSetDepthBias = 96,
			vkCmdSetBlendConstants = 97,
			vkCmdSetDepthBounds = 98,
			vkCmdSetStencilCompareMask = 99,
			vkCmdSetStencilWriteMask = 100,
			vkCmdSetStencilReference = 101,
			vkCmdBindDescriptorSets = 102,
			vkCmdBindIndexBuffer = 103,
			vkCmdBindVertexBuffers = 104,
			vkCmdDraw = 105,
			vkCmdDrawIndexed = 106,
			vkCmdDrawIndirect = 107,
			vkCmdDrawIndexedIndirect = 108,
			vkCmdDispatch = 109,
			vkCmdDispatchIndirect = 110,
			vkCmdCopyBuffer = 111,
			vkCmdCopyImage = 112,
			vkCmdBlitImage = 113,
			vkCmdCopyBufferToImage = 114,
			vkCmdCopyImageToBuffer = 115,
			vkCmdUpdateBuffer = 116,
			vkCmdFillBuffer = 117,
			vkCmdClearColorImage = 118,
			vkCmdClearDepthStencilImage = 119,
			vkCmdClearAttachments = 120,
			vkCmdResolveImage = 121,
			vkCmdSetEvent = 122,
			vkCmdResetEvent = 123,
			vkCmdWaitEvents = 124,
			vkCmdPipelineBarrier = 125,
			vkCmdBeginQuery = 126,
			vkCmdEndQuery = 127,
			vkCmdResetQueryPool = 128,
			vkCmdWriteTimestamp = 129,
			vkCmdCopyQueryPoolResults = 130,
			vkCmdPushConstants = 131,
			vkCmdBeginRenderPass = 132,
			vkCmdNextSubpass = 133,
			vkCmdEndRenderPass = 134,
			vkCmdExecuteCommands = 135,
		};
	};
}

#endif // __KIERO_H__