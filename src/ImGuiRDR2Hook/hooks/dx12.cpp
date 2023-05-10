// Code modified from https://github.com/Sh0ckFR/Universal-Dear-ImGui-Hook/blob/master/d3d12hook.cpp
// Big thanks to Sh0ckFR

#include "dx12.h"
#include "win32.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#pragma comment(lib, "libMinHook.x64.lib")

ID3D12Device* g_d3d12Device = nullptr;
ID3D12DescriptorHeap* g_d3d12DescriptorHeapBackBuffers = nullptr;
ID3D12DescriptorHeap* g_d3d12DescriptorHeapImGuiRender = nullptr;
ID3D12GraphicsCommandList* g_d3d12CommandList = nullptr;
ID3D12CommandQueue* g_d3d12CommandQueue = nullptr;

// string form of the IID for the ID3D12Device interface
struct __declspec(uuid("189819F1-1DB6-4B57-BE54-1821339B85F7")) ID3D12Device;

struct FrameContext
{
	ID3D12CommandAllocator* pCommandAllocator = nullptr;
	ID3D12Resource* pRenderTargetResource = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE sRenderTargetDescriptor{};
};

UINT uBuffersCounts = 0;
FrameContext* pFrameContext;

typedef long(__fastcall* Present_t) (IDXGISwapChain*, UINT, UINT);
Present_t og_Present{};
long __fastcall hk_Present(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!hooks::g_bImGuiInitialized)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&g_d3d12Device)))
		{
			Log("[+] DX12: winerror.h SUCCEEDED() - Pass");
			hooks::g_hWnd = FindWindowA("sgaWindow", "Red Dead Redemption 2");

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();

			unsigned char* pixels;
			int width, height;
			ImGuiIO& io = ImGui::GetIO();
			(void)io;
			ImGui::StyleColorsDark();
			io.Fonts->AddFontDefault();
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
			io.IniFilename = NULL;

			CreateEvent(nullptr, false, false, nullptr);

			DXGI_SWAP_CHAIN_DESC sdesc;
			pSwapChain->GetDesc(&sdesc);
			sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			sdesc.OutputWindow = hooks::g_hWnd;
			sdesc.Windowed = ((GetWindowLongPtr(hooks::g_hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;

			uBuffersCounts = sdesc.BufferCount;
			pFrameContext = new FrameContext[uBuffersCounts];

			D3D12_DESCRIPTOR_HEAP_DESC descriptorImGuiRender = {};
			descriptorImGuiRender.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			descriptorImGuiRender.NumDescriptors = uBuffersCounts;
			descriptorImGuiRender.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			if (g_d3d12Device->CreateDescriptorHeap(&descriptorImGuiRender, IID_PPV_ARGS(&g_d3d12DescriptorHeapImGuiRender)) != S_OK) {
				Log("[!] DX12: CreateDescriptorHeap(descriptorImGuiRender) FAILED");
				return false;
			}

			ID3D12CommandAllocator* allocator;
			if (g_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)) != S_OK) {
				Log("[!] DX12: CreateCommandAllocator() FAILED");
				return false;
			}

			for (size_t i = 0; i < uBuffersCounts; i++) {
				pFrameContext[i].pCommandAllocator = allocator;
			}

			if (g_d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator, NULL, IID_PPV_ARGS(&g_d3d12CommandList)) != S_OK ||
				g_d3d12CommandList->Close() != S_OK)
			{
				Log("[!] DX12: CreateCommandList() FAILED");
				return false;
			}


			D3D12_DESCRIPTOR_HEAP_DESC descriptorBackBuffers;
			descriptorBackBuffers.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			descriptorBackBuffers.NumDescriptors = uBuffersCounts;
			descriptorBackBuffers.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			descriptorBackBuffers.NodeMask = 1;

			if (g_d3d12Device->CreateDescriptorHeap(&descriptorBackBuffers, IID_PPV_ARGS(&g_d3d12DescriptorHeapBackBuffers)) != S_OK) {
				Log("[!] DX12: CreateDescriptorHeap(descriptorBackBuffers) FAILED");
				return false;
			}

			const auto rtvDescriptorSize = g_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_d3d12DescriptorHeapBackBuffers->GetCPUDescriptorHandleForHeapStart();

			for (size_t i = 0; i < uBuffersCounts; i++) {
				ID3D12Resource* pBackBuffer = nullptr;

				pFrameContext[i].sRenderTargetDescriptor = rtvHandle;
				pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
				g_d3d12Device->CreateRenderTargetView(pBackBuffer, nullptr, rtvHandle);
				pFrameContext[i].pRenderTargetResource = pBackBuffer;
				rtvHandle.ptr += rtvDescriptorSize;
			}

			ImGui_ImplWin32_Init(hooks::g_hWnd);
			ImGui_ImplDX12_Init
			(
				g_d3d12Device,
				uBuffersCounts,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				g_d3d12DescriptorHeapImGuiRender,
				g_d3d12DescriptorHeapImGuiRender->GetCPUDescriptorHandleForHeapStart(),
				g_d3d12DescriptorHeapImGuiRender->GetGPUDescriptorHandleForHeapStart()
			);
			ImGui_ImplDX12_CreateDeviceObjects();

			hooks::win32::Hook();
		}
		else
		{
			Log("[!] DX12: winerror.h SUCCEEDED() - FAILED");
		}

		hooks::g_bImGuiInitialized = true;
	}

	if (!hooks::g_bShutdownRequested)
	{
		if (g_d3d12CommandQueue == nullptr) {
			return og_Present(pSwapChain, SyncInterval, Flags);
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

		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		CMenu::Draw();

		FrameContext& currentFrameContext = pFrameContext[pSwapChain->GetCurrentBackBufferIndex()];
		currentFrameContext.pCommandAllocator->Reset();

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = currentFrameContext.pRenderTargetResource;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		g_d3d12CommandList->Reset(currentFrameContext.pCommandAllocator, nullptr);
		g_d3d12CommandList->ResourceBarrier(1, &barrier);
		g_d3d12CommandList->OMSetRenderTargets(1, &currentFrameContext.sRenderTargetDescriptor, FALSE, nullptr);
		g_d3d12CommandList->SetDescriptorHeaps(1, &g_d3d12DescriptorHeapImGuiRender);

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_d3d12CommandList);

		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		g_d3d12CommandList->ResourceBarrier(1, &barrier);
		g_d3d12CommandList->Close();

		g_d3d12CommandQueue->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(&g_d3d12CommandList));
	}

	return og_Present(pSwapChain, SyncInterval, Flags);
}

typedef void(*ExecuteCommandLists_t) (ID3D12CommandQueue*, UINT, ID3D12CommandList*);
ExecuteCommandLists_t og_ExecuteCommandLists{};
void hk_ExecuteCommandLists(ID3D12CommandQueue* pQueue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists)
{
	if (!g_d3d12CommandQueue) {
		g_d3d12CommandQueue = pQueue;
	}

	og_ExecuteCommandLists(pQueue, NumCommandLists, ppCommandLists);
}


namespace hooks {
namespace dx12 {

	void Hook()
	{
		if (GetModuleHandleA("d3d12.dll") == NULL) {
			Log("[!] DX12: Cannot hook: d3d12.dll is not loaded.");
			return;
		}

		kiero::Status::Enum initStatus = kiero::init(kiero::RenderType::D3D12);
		kiero::Status::Enum bindStatus;
		if (initStatus == kiero::Status::Success)
		{
			Log("[+] DX12: kiero::init(D3D12) - Success");

			bindStatus = kiero::bind(kiero::D3D12MT::ExecuteCommandLists, (void**)&og_ExecuteCommandLists, hk_ExecuteCommandLists);
			Log("[+] DX12: bind (ExecuteCommandLists) - %s", KieroStatusEnumToString(bindStatus));
			bindStatus = kiero::bind(kiero::D3D12MT::Present, (void**)&og_Present, hk_Present);
			Log("[+] DX12: bind (Present) - %s", KieroStatusEnumToString(bindStatus));

			Log("[+] DX12: kiero::bind() functions completed");
		}
		else
		{
			Log("[!] DX12: kiero::init(D3D12) - FAILED - %s", KieroStatusEnumToString(initStatus));
		}
	}

	void Unhook()
	{
		Log("[!] DX12: Unhooking...");

		g_bShutdownRequested = true;

		if (g_d3d12Device) g_d3d12Device->Release();
		if (g_d3d12DescriptorHeapBackBuffers) g_d3d12DescriptorHeapBackBuffers->Release();
		if (g_d3d12DescriptorHeapImGuiRender) g_d3d12DescriptorHeapImGuiRender->Release();
		if (g_d3d12CommandList) g_d3d12CommandList->Release();
		if (g_d3d12CommandQueue) g_d3d12CommandQueue->Release();

		kiero::shutdown();
		if (ImGui::GetCurrentContext())
		{
			if (ImGui::GetIO().BackendRendererUserData)
				ImGui_ImplDX12_Shutdown();

			if (ImGui::GetIO().BackendPlatformUserData)
				ImGui_ImplWin32_Shutdown();

			ImGui::DestroyContext();
		}
		hooks::win32::Unhook();
	}

} // namespace dx12
} // namespace hooks

