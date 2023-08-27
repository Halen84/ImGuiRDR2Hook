# ImGuiRDR2Hook
 A Vulkan & DirectX 12 hook that allows you to create ImGui menus for RDR2
 
# Prerequisites
- [MinHook x64](https://github.com/TsudaKageyu/minhook) (Already included)
- [Kiero](https://github.com/Rebzzel/kiero) (Already included)
- [ImGui](https://github.com/ocornut/imgui) (Already included)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

# How To Use
Before trying to compile the main project, you must first compile MinHook (libMinHook) so the main project can reference some required files.
There's really not much to set up, but you must first make sure you have Vulkan SDK installed, which can be found here: [https://vulkan.lunarg.com/sdk/home](https://vulkan.lunarg.com/sdk/home). You may need to restart Visual Studio after installing the Vulkan SDK.

- **Note: <u>Use WaitAndRender() instead of WAIT() to prevent a crash!</u> This function renders ImGui while yielding. Also see https://github.com/Halen84/ImGuiRDR2Hook/issues/2**
- Do not touch anything in the `hooks` folder unless you know what you're doing!
- script.cpp still contains the main loop for the script.
- menu.cpp/.h is where the ImGui menu is drawn.
- main.cpp is where the hook functions are called
- See hooks/vulkan.cpp --> RenderImGui_Vulkan() for drawing the mouse cursor and changing the style in Vulkan
- See hooks/dx12.cpp --> hk_Present() for drawing the mouse cursor and changing the style in DirectX 12
 
# Credits
- [Sh0ckFR/Universal-Dear-ImGui-Hook](https://github.com/Sh0ckFR/Universal-Dear-ImGui-Hook) (DirectX 12 Code)
- [bruhmoment21/UniversalHookX](https://github.com/bruhmoment21/UniversalHookX) (Vulkan Code)
