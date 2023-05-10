#include "menu.h"

bool CMenu::bIsOpen = false;

void CMenu::Draw()
{
	if (bIsOpen)
	{
		ImGui::Begin("ImGui Menu", &bIsOpen, bIsOpen ? 0 : ImGuiWindowFlags_NoMouseInputs);

		// Start here
		ImGui::Text("Sample Text");

		ImGui::End();
	}
}
