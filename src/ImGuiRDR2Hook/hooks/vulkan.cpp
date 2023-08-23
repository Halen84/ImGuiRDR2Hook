// Code modified from: https://github.com/bruhmoment21/UniversalHookX/blob/main/UniversalHookX/src/hooks/backend/vulkan/hook_vulkan.cpp
// Big thanks to bruhmoment21

#include "vulkan.h"
#include "win32.h"
#include <vector>
#include <vulkan/vulkan.h>
#pragma comment(lib, "vulkan-1.lib")

VkAllocationCallbacks* g_Allocator = NULL;
VkInstance g_Instance = VK_NULL_HANDLE;
VkPhysicalDevice g_PhysicalDevice = VK_NULL_HANDLE;
VkDevice g_FakeDevice = VK_NULL_HANDLE;
VkDevice g_Device = VK_NULL_HANDLE;

uint32_t g_QueueFamily = (uint32_t)-1;
std::vector<VkQueueFamilyProperties> g_QueueFamilies;

VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;
VkDescriptorPool g_DescriptorPool = VK_NULL_HANDLE;
uint32_t g_MinImageCount = 2;
VkRenderPass g_RenderPass = VK_NULL_HANDLE;
ImGui_ImplVulkanH_Frame g_Frames[8] = {};
ImGui_ImplVulkanH_FrameSemaphores g_FrameSemaphores[8] = {};
VkExtent2D g_ImageExtent = {};


bool CreateDeviceVK()
{
	// Create Vulkan Instance
	{
		VkInstanceCreateInfo create_info = {};
		constexpr const char* instance_extension = "VK_KHR_surface";

		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.enabledExtensionCount = 1;
		create_info.ppEnabledExtensionNames = &instance_extension;

		// Create Vulkan Instance without any debug feature
		if (vkCreateInstance(&create_info, g_Allocator, &g_Instance) != VK_SUCCESS) {
			Log("[!] Vulkan: vkCreateInstance() FAILED for g_Instance");
			return false;
		}
		Log("[+] Vulkan: g_Instance: 0x%p", g_Instance);
	}

	// Select GPU
	{
		uint32_t gpu_count;
		vkEnumeratePhysicalDevices(g_Instance, &gpu_count, NULL);
		IM_ASSERT(gpu_count > 0);

		VkPhysicalDevice* gpus = new VkPhysicalDevice[sizeof(VkPhysicalDevice) * gpu_count];
		vkEnumeratePhysicalDevices(g_Instance, &gpu_count, gpus);

		// If a number >1 of GPUs got reported, find discrete GPU if present, or use first one available. This covers
		// most common cases (multi-gpu/integrated+dedicated graphics). Handling more complicated setups (multiple
		// dedicated GPUs) is out of scope of this sample.
		int use_gpu = 0;
		for (int i = 0; i < (int)gpu_count; ++i) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(gpus[i], &properties);
			if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				use_gpu = i;
				break;
			}
		}

		g_PhysicalDevice = gpus[use_gpu];
		Log("[+] Vulkan: g_PhysicalDevice: 0x%p", g_PhysicalDevice);

		delete[] gpus;
	}

	// Select graphics queue family
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, NULL);
		g_QueueFamilies.resize(count);
		vkGetPhysicalDeviceQueueFamilyProperties(g_PhysicalDevice, &count, g_QueueFamilies.data());
		for (uint32_t i = 0; i < count; ++i) {
			if (g_QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				g_QueueFamily = i;
				break;
			}
		}
		IM_ASSERT(g_QueueFamily != (uint32_t)-1);

		Log("[+] Vulkan: g_QueueFamily: %u", g_QueueFamily);
	}

	// Create Logical Device (with 1 queue)
	{
		constexpr const char* device_extension = "VK_KHR_swapchain";
		constexpr const float queue_priority = 1.0f;

		VkDeviceQueueCreateInfo queue_info = {};
		queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info.queueFamilyIndex = g_QueueFamily;
		queue_info.queueCount = 1;
		queue_info.pQueuePriorities = &queue_priority;

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = 1;
		create_info.pQueueCreateInfos = &queue_info;
		create_info.enabledExtensionCount = 1;
		create_info.ppEnabledExtensionNames = &device_extension;

		if (vkCreateDevice(g_PhysicalDevice, &create_info, g_Allocator, &g_FakeDevice) != VK_SUCCESS) {
			Log("[!] Vulkan: vkCreateDevice() FAILED for g_FakeDevice");
			return false;
		}
		Log("[+] Vulkan: g_FakeDevice: 0x%p", g_FakeDevice);
	}

	return true;
}

void CreateRenderTarget(VkDevice device, VkSwapchainKHR swapchain)
{
	uint32_t uImageCount;
	vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, NULL);

	VkImage backbuffers[8] = {};
	vkGetSwapchainImagesKHR(device, swapchain, &uImageCount, backbuffers);

	for (uint32_t i = 0; i < uImageCount; ++i) {
		g_Frames[i].Backbuffer = backbuffers[i];

		ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
		ImGui_ImplVulkanH_FrameSemaphores* fsd = &g_FrameSemaphores[i];
		{
			VkCommandPoolCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			info.queueFamilyIndex = g_QueueFamily;

			vkCreateCommandPool(device, &info, g_Allocator, &fd->CommandPool);
		}
		{
			VkCommandBufferAllocateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.commandPool = fd->CommandPool;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			info.commandBufferCount = 1;

			vkAllocateCommandBuffers(device, &info, &fd->CommandBuffer);
		}
		{
			VkFenceCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			vkCreateFence(device, &info, g_Allocator, &fd->Fence);
		}
		{
			VkSemaphoreCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vkCreateSemaphore(device, &info, g_Allocator, &fsd->ImageAcquiredSemaphore);
			vkCreateSemaphore(device, &info, g_Allocator, &fsd->RenderCompleteSemaphore);
		}
	}

	// Create the Render Pass
	{
		VkAttachmentDescription attachment = {};
		attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &color_attachment;

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;

		vkCreateRenderPass(device, &info, g_Allocator, &g_RenderPass);
	}

	// Create The Image Views
	{
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.format = VK_FORMAT_B8G8R8A8_UNORM;

		info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;

		for (uint32_t i = 0; i < uImageCount; ++i) {
			ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
			info.image = fd->Backbuffer;

			vkCreateImageView(device, &info, g_Allocator, &fd->BackbufferView);
		}
	}

	// Create Framebuffer
	{
		VkImageView attachment[1];
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = g_RenderPass;
		info.attachmentCount = 1;
		info.pAttachments = attachment;
		info.layers = 1;

		for (uint32_t i = 0; i < uImageCount; ++i) {
			ImGui_ImplVulkanH_Frame* fd = &g_Frames[i];
			attachment[0] = fd->BackbufferView;

			vkCreateFramebuffer(device, &info, g_Allocator, &fd->Framebuffer);
		}
	}

	// Create Descriptor Pool.
	if (!g_DescriptorPool)
	{
		constexpr VkDescriptorPoolSize pool_sizes[] =
		{
			{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		vkCreateDescriptorPool(device, &pool_info, g_Allocator, &g_DescriptorPool);
	}
}

void CleanupRenderTarget()
{
	for (uint32_t i = 0; i < RTL_NUMBER_OF(g_Frames); ++i) {
		if (g_Frames[i].Fence) {
			vkDestroyFence(g_Device, g_Frames[i].Fence, g_Allocator);
			g_Frames[i].Fence = VK_NULL_HANDLE;
		}
		if (g_Frames[i].CommandBuffer) {
			vkFreeCommandBuffers(g_Device, g_Frames[i].CommandPool, 1, &g_Frames[i].CommandBuffer);
			g_Frames[i].CommandBuffer = VK_NULL_HANDLE;
		}
		if (g_Frames[i].CommandPool) {
			vkDestroyCommandPool(g_Device, g_Frames[i].CommandPool, g_Allocator);
			g_Frames[i].CommandPool = VK_NULL_HANDLE;
		}
		if (g_Frames[i].BackbufferView) {
			vkDestroyImageView(g_Device, g_Frames[i].BackbufferView, g_Allocator);
			g_Frames[i].BackbufferView = VK_NULL_HANDLE;
		}
		if (g_Frames[i].Framebuffer) {
			vkDestroyFramebuffer(g_Device, g_Frames[i].Framebuffer, g_Allocator);
			g_Frames[i].Framebuffer = VK_NULL_HANDLE;
		}
	}

	for (uint32_t i = 0; i < RTL_NUMBER_OF(g_FrameSemaphores); ++i) {
		if (g_FrameSemaphores[i].ImageAcquiredSemaphore) {
			vkDestroySemaphore(g_Device, g_FrameSemaphores[i].ImageAcquiredSemaphore, g_Allocator);
			g_FrameSemaphores[i].ImageAcquiredSemaphore = VK_NULL_HANDLE;
		}
		if (g_FrameSemaphores[i].RenderCompleteSemaphore) {
			vkDestroySemaphore(g_Device, g_FrameSemaphores[i].RenderCompleteSemaphore, g_Allocator);
			g_FrameSemaphores[i].RenderCompleteSemaphore = VK_NULL_HANDLE;
		}
	}
}

void CleanupDeviceVulkan()
{
	CleanupRenderTarget();

	if (g_DescriptorPool) {
		vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
		g_DescriptorPool = NULL;
	}
	if (g_Instance) {
		vkDestroyInstance(g_Instance, g_Allocator);
		g_Instance = NULL;
	}

	g_ImageExtent = {};
	g_Device = NULL;
}

bool DoesQueueSupportGraphic(VkQueue queue, VkQueue* pGraphicQueue)
{
	for (uint32_t i = 0; i < g_QueueFamilies.size(); ++i) {
		const VkQueueFamilyProperties& family = g_QueueFamilies[i];
		for (uint32_t j = 0; j < family.queueCount; ++j) {
			VkQueue it = VK_NULL_HANDLE;
			vkGetDeviceQueue(g_Device, i, j, &it);

			if (pGraphicQueue && family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				if (*pGraphicQueue == VK_NULL_HANDLE) {
					*pGraphicQueue = it;
				}
			}

			if (queue == it && family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				return true;
			}
		}
	}

	return false;
}

void hooks::vulkan::RenderImGui_Vulkan(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
{
	if (!g_Device || hooks::bShutdownRequested)
		return;

	VkQueue graphicQueue = VK_NULL_HANDLE;
	const bool queueSupportsGraphic = DoesQueueSupportGraphic(queue, &graphicQueue);

	if (!ImGui::GetCurrentContext()) {
		hooks::hWnd = FindWindowA("sgaWindow", "Red Dead Redemption 2");
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hooks::hWnd);
		hooks::win32::Hook();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = io.LogFilename = NULL;
		ImGui::StyleColorsDark();
	}

	for (uint32_t i = 0; i < pPresentInfo->swapchainCount; ++i)
	{
		VkSwapchainKHR swapchain = pPresentInfo->pSwapchains[i];
		if (g_Frames[0].Framebuffer == VK_NULL_HANDLE) {
			CreateRenderTarget(g_Device, swapchain);
		}

		ImGui_ImplVulkanH_Frame* fd = &g_Frames[pPresentInfo->pImageIndices[i]];
		ImGui_ImplVulkanH_FrameSemaphores* fsd = &g_FrameSemaphores[pPresentInfo->pImageIndices[i]];
		{
			vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, ~0ull);
			vkResetFences(g_Device, 1, &fd->Fence);
		}
		{
			vkResetCommandBuffer(fd->CommandBuffer, 0);

			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			vkBeginCommandBuffer(fd->CommandBuffer, &info);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = g_RenderPass;
			info.framebuffer = fd->Framebuffer;
			if (g_ImageExtent.width == 0 || g_ImageExtent.height == 0) {
				// We don't know the window size the first time. So we just set it to 4K.
				// TODO: Maybe default to 1920x1080
				info.renderArea.extent.width = 3840;
				info.renderArea.extent.height = 2160;
			} else {
				info.renderArea.extent = g_ImageExtent;
			}

			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		if (!ImGui::GetIO().BackendRendererUserData)
		{
			ImGui_ImplVulkan_InitInfo init_info = {};
			init_info.Instance = g_Instance;
			init_info.PhysicalDevice = g_PhysicalDevice;
			init_info.Device = g_Device;
			init_info.QueueFamily = g_QueueFamily;
			init_info.Queue = graphicQueue;
			init_info.PipelineCache = g_PipelineCache;
			init_info.DescriptorPool = g_DescriptorPool;
			init_info.Subpass = 0;
			init_info.MinImageCount = g_MinImageCount;
			init_info.ImageCount = g_MinImageCount;
			init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
			init_info.Allocator = g_Allocator;
			ImGui_ImplVulkan_Init(&init_info, g_RenderPass);
			ImGui_ImplVulkan_CreateFontsTexture(fd->CommandBuffer);
		}

		ImGuiIO& io = ImGui::GetIO();
		if (CMenu::bIsOpen) {
			io.WantCaptureMouse = true;
			io.MouseDrawCursor = true;
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else {
			io.WantCaptureMouse = false;
			io.MouseDrawCursor = false;
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		CMenu::Draw();
		ImGui::Render();

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		vkEndCommandBuffer(fd->CommandBuffer);

		uint32_t waitSemaphoresCount = i == 0 ? pPresentInfo->waitSemaphoreCount : 0;
		if (waitSemaphoresCount == 0 && !queueSupportsGraphic)
		{
			constexpr VkPipelineStageFlags stages_wait = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
			{
				VkSubmitInfo info = {};
				info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

				info.pWaitDstStageMask = &stages_wait;

				info.signalSemaphoreCount = 1;
				info.pSignalSemaphores = &fsd->RenderCompleteSemaphore;

				vkQueueSubmit(queue, 1, &info, VK_NULL_HANDLE);
			}
			{
				VkSubmitInfo info = {};
				info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				info.commandBufferCount = 1;
				info.pCommandBuffers = &fd->CommandBuffer;

				info.pWaitDstStageMask = &stages_wait;
				info.waitSemaphoreCount = 1;
				info.pWaitSemaphores = &fsd->RenderCompleteSemaphore;

				info.signalSemaphoreCount = 1;
				info.pSignalSemaphores = &fsd->ImageAcquiredSemaphore;

				vkQueueSubmit(graphicQueue, 1, &info, fd->Fence);
			}
		}
		else
		{
			std::vector<VkPipelineStageFlags> stages_wait(waitSemaphoresCount, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;

			info.pWaitDstStageMask = stages_wait.data();
			info.waitSemaphoreCount = waitSemaphoresCount;
			info.pWaitSemaphores = pPresentInfo->pWaitSemaphores;

			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &fsd->ImageAcquiredSemaphore;

			vkQueueSubmit(graphicQueue, 1, &info, fd->Fence);
		}
	}
}

std::add_pointer_t<VkResult VKAPI_CALL(VkDevice, VkSwapchainKHR, uint64_t, VkSemaphore, VkFence, uint32_t*)> oAcquireNextImageKHR;
VkResult VKAPI_CALL hk_vkAcquireNextImageKHR(VkDevice device, VkSwapchainKHR swapchain, uint64_t timeout, VkSemaphore semaphore, VkFence fence, uint32_t* pImageIndex)
{
	g_Device = device;

	return oAcquireNextImageKHR(device, swapchain, timeout, semaphore, fence, pImageIndex);
}

std::add_pointer_t<VkResult VKAPI_CALL(VkDevice, const VkAcquireNextImageInfoKHR*, uint32_t*)> oAcquireNextImage2KHR;
VkResult VKAPI_CALL hk_vkAcquireNextImage2KHR(VkDevice device, const VkAcquireNextImageInfoKHR* pAcquireInfo, uint32_t* pImageIndex)
{
	g_Device = device;

	return oAcquireNextImage2KHR(device, pAcquireInfo, pImageIndex);
}

std::add_pointer_t<VkResult VKAPI_CALL(VkQueue, const VkPresentInfoKHR*)> oQueuePresentKHR;
VkResult VKAPI_CALL hk_vkQueuePresentKHR(VkQueue queue, const VkPresentInfoKHR* pPresentInfo)
{
	hooks::vulkan::m_queue = queue;
	hooks::vulkan::m_pPresentInfo = pPresentInfo;
	hooks::vulkan::RenderImGui_Vulkan(queue, pPresentInfo);

	return oQueuePresentKHR(queue, pPresentInfo);
}

std::add_pointer_t<VkResult VKAPI_CALL(VkDevice, const VkSwapchainCreateInfoKHR*, const VkAllocationCallbacks*, VkSwapchainKHR*)> oCreateSwapchainKHR;
VkResult VKAPI_CALL hk_vkCreateSwapchainKHR(VkDevice device, const VkSwapchainCreateInfoKHR* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSwapchainKHR* pSwapchain)
{
	CleanupRenderTarget();
	g_ImageExtent = pCreateInfo->imageExtent;

	return oCreateSwapchainKHR(device, pCreateInfo, pAllocator, pSwapchain);
}


namespace hooks {
namespace vulkan {

	void Hook()
	{
		if (GetModuleHandleA("vulkan-1.dll") == NULL) {
			Log("[!] Vulkan: Cannot hook: vulkan-1.dll is not loaded.");
			return;
		}

		if (!CreateDeviceVK()) {
			Log("[!] Vulkan: CreateDeviceVK() failed.");
			return;
		}

		MH_Initialize();

		void* fpAcquireNextImageKHR = reinterpret_cast<void*>(vkGetDeviceProcAddr(g_FakeDevice, "vkAcquireNextImageKHR"));
		void* fpAcquireNextImage2KHR = reinterpret_cast<void*>(vkGetDeviceProcAddr(g_FakeDevice, "vkAcquireNextImage2KHR"));
		void* fpQueuePresentKHR = reinterpret_cast<void*>(vkGetDeviceProcAddr(g_FakeDevice, "vkQueuePresentKHR"));
		void* fpCreateSwapchainKHR = reinterpret_cast<void*>(vkGetDeviceProcAddr(g_FakeDevice, "vkCreateSwapchainKHR"));

		// FIXME: Calling vkDestroyDevice() anywhere will causes RDR2 to hang - Unfixable?
		/*if (g_FakeDevice) {
			vkDestroyDevice(g_FakeDevice, g_Allocator);
			g_FakeDevice = NULL;
		}*/

		if (fpAcquireNextImageKHR) {
			Log("[+] Vulkan: fpAcquireNextImageKHR:  0x%p", fpAcquireNextImageKHR);
			Log("[+] Vulkan: fpAcquireNextImage2KHR: 0x%p", fpAcquireNextImage2KHR);
			Log("[+] Vulkan: fpQueuePresentKHR:      0x%p", fpQueuePresentKHR);
			Log("[+] Vulkan: fpCreateSwapchainKHR:   0x%p", fpCreateSwapchainKHR);

			MH_STATUS aniStatus = MH_CreateHook(reinterpret_cast<void**>(fpAcquireNextImageKHR), &hk_vkAcquireNextImageKHR, reinterpret_cast<void**>(&oAcquireNextImageKHR));
			MH_STATUS ani2Status = MH_CreateHook(reinterpret_cast<void**>(fpAcquireNextImage2KHR), &hk_vkAcquireNextImage2KHR, reinterpret_cast<void**>(&oAcquireNextImage2KHR));
			MH_STATUS qpStatus = MH_CreateHook(reinterpret_cast<void**>(fpQueuePresentKHR), &hk_vkQueuePresentKHR, reinterpret_cast<void**>(&oQueuePresentKHR));
			MH_STATUS csStatus = MH_CreateHook(reinterpret_cast<void**>(fpCreateSwapchainKHR), &hk_vkCreateSwapchainKHR, reinterpret_cast<void**>(&oCreateSwapchainKHR));
			Log("[+] Vulkan: MH_CreateHook() aniStatus:  %s", MHStatusToString(aniStatus));
			Log("[+] Vulkan: MH_CreateHook() ani2Status: %s", MHStatusToString(ani2Status));
			Log("[+] Vulkan: MH_CreateHook() qpStatus:   %s", MHStatusToString(qpStatus));
			Log("[+] Vulkan: MH_CreateHook() csStatus:   %s", MHStatusToString(csStatus));

			aniStatus = MH_EnableHook(fpAcquireNextImageKHR);
			ani2Status = MH_EnableHook(fpAcquireNextImage2KHR);
			qpStatus = MH_EnableHook(fpQueuePresentKHR);
			csStatus = MH_EnableHook(fpCreateSwapchainKHR);
			Log("[+] Vulkan: MH_EnableHook() aniStatus:  %s", MHStatusToString(aniStatus));
			Log("[+] Vulkan: MH_EnableHook() ani2Status: %s", MHStatusToString(ani2Status));
			Log("[+] Vulkan: MH_EnableHook() qpStatus:   %s", MHStatusToString(qpStatus));
			Log("[+] Vulkan: MH_EnableHook() csStatus:   %s", MHStatusToString(csStatus));
		}
	}

	void Unhook()
	{
		Log("[!] Vulkan: Unhooking...");

		bShutdownRequested = true;

		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().BackendRendererUserData)
				ImGui_ImplVulkan_Shutdown();

			if (ImGui::GetIO().BackendPlatformUserData)
				ImGui_ImplWin32_Shutdown();

			ImGui::DestroyContext();
		}
		hooks::win32::Unhook();
		MH_DisableHook(MH_ALL_HOOKS);

		CleanupDeviceVulkan();
	}

} // namespace vulkan
} // namespace hooks
