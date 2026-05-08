# ImGuiRDR2Hook
 A Vulkan & DirectX 12 hook that allows you to create ImGui menus for RDR2

<details>
  <summary>In game image</summary>
  <img width="1351" height="782" alt="image" src="https://github.com/user-attachments/assets/d8ef6bd2-b64f-48c5-8b74-e67f9d15b78d" />
</details>
 
# Prerequisites
- [MinHook x64](https://github.com/TsudaKageyu/minhook) (Already included)
- [Kiero](https://github.com/Rebzzel/kiero) (Already included)
- [ImGui](https://github.com/ocornut/imgui) (Already included)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

# How To Use
Before trying to compile the main project, you must first compile MinHook (libMinHook) so the main project can reference some required files.
There's really not much to set up, but you must first make sure you have Vulkan SDK installed, which can be found here: [https://vulkan.lunarg.com/sdk/home](https://vulkan.lunarg.com/sdk/home). You may need to restart Visual Studio after installing the Vulkan SDK.

- To open the menu, the default open key is `F10`. To toggle the mouse, press the mouse back button.
- If you wish to change the toggle key or edit mouse behavior, see `Win32.cpp`.
- `script.cpp` still contains the main loop for the script.
- `menu.cpp/.h` is where the ImGui menu is rendered.
- `main.cpp` contains code that reads config files + ScriptHook registry
 
# Credits
- [Sh0ckFR/Universal-Dear-ImGui-Hook](https://github.com/Sh0ckFR/Universal-Dear-ImGui-Hook) (DirectX 12 Code)
- [bruhmoment21/UniversalHookX](https://github.com/bruhmoment21/UniversalHookX) (Vulkan Code)
