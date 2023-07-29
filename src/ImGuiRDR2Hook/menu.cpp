// Licensed under the MIT License - Halen84 (TuffyTown)

#include "menu.h"

bool CMenu::bIsOpen = false;

char arBuffer[256]{};
float fFloatValue = 0.0f;

void CMenu::Draw()
{
	if (bIsOpen)
	{
		// Comment this out if you do NOT want in-game controls
		// to be disabled while the ImGui menu is open.
		DisableAllControlActionsThisFrame();

		ImGui::Begin("ImGui Menu", &bIsOpen, bIsOpen ? 0 : ImGuiWindowFlags_NoMouseInputs);

		ImGui::Text("Hello, world %d", 123);
		if (ImGui::Button("Button")) {
			// Do something when button is pressed
		}
		ImGui::InputText("string", arBuffer, IM_ARRAYSIZE(arBuffer));
		ImGui::SliderFloat("float", &fFloatValue, 0.0f, 1.0f);

		ImGui::End();
	}
}
