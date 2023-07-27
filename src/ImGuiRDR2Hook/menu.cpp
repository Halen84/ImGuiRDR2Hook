// Licensed under the MIT License - Halen84 (TuffyTown)

#include "menu.h"

bool CMenu::bIsOpen = false;

namespace ImGuiVars
{
	char buffer[256]{};
	float floatVal = 0.0f;
}

void CMenu::Draw()
{
	if (bIsOpen)
	{
		ImGui::Begin("ImGui Menu", &bIsOpen, bIsOpen ? 0 : ImGuiWindowFlags_NoMouseInputs);

		ImGui::Text("Hello, world %d", 123);
		if (ImGui::Button("Button")) {
			// Do something when button is pressed
		}
		ImGui::InputText("string", ImGuiVars::buffer, IM_ARRAYSIZE(ImGuiVars::buffer));
		ImGui::SliderFloat("float", &ImGuiVars::floatVal, 0.0f, 1.0f);

		ImGui::End();
	}
}
