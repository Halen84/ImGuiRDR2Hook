#include "menu.h"

bool CImGuiMenu::sm_bMenuOpen = true;
bool CImGuiMenu::sm_bDrawMouse = true;

char s_StringBuffer[256] = {};
float s_fFloatValue = 0.0f;

void CImGuiMenu::Render()
{
	if (GetIsOpen())
	{
		ImGui::SetNextWindowSize({250, 200}, ImGuiCond_Once);
		if (ImGui::Begin("ImGui Menu", &sm_bMenuOpen, sm_bMenuOpen ? 0 : ImGuiWindowFlags_NoMouseInputs))
		{
			ImGui::Text("Hello, world %d", 123);
			ImGui::Button("Button");
			ImGui::InputText("string", s_StringBuffer, IM_ARRAYSIZE(s_StringBuffer));
			ImGui::SliderFloat("float", &s_fFloatValue, 0.0f, 1.0f);
		}
		ImGui::End();
	}
}
