#include "Menu.h"

void CImGuiMenu::Render()
{
	if (ImGui::IsKeyReleased(ImGuiKey_F10)) {
		m_bIsOpen ^= true;
	}

	if (!m_bIsOpen) {
		return;
	}

	static char Buffer[128] = "Hello, world!";
	static float FloatValue = 0.25f;

	ImGui::SetNextWindowSize({250, 200}, ImGuiCond_Once);
	// Appending "##Example" to the window name to prevent conflicts with other windows from other scripts with the same name
	if (ImGui::Begin("ImGui Menu##Example", &m_bIsOpen))
	{
		ImGui::Text("Hello, world %d", 123);
		ImGui::Button("Button");
		ImGui::InputText("string", Buffer, IM_ARRAYSIZE(Buffer));
		ImGui::SliderFloat("float", &FloatValue, 0.0f, 1.0f);
	}
	ImGui::End();
}
