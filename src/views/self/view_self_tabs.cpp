#include "pointers.hpp"
#include "views/view.hpp"

namespace big
{
	void view::self_tabs()
	{
		if (ImGui::BeginTabBar("self_tabbar"))
		{
			if (ImGui::BeginTabItem("Self"))
			{
				view::self();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Weapons"))
			{
				view::weapons();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Mobile"))
			{
				view::mobile();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Teleport"))
			{
				view::teleport();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Outfit Editor"))
			{
				view::outfit_editor();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Outfit Slots"))
			{
				view::outfit_slots();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
}
