/**
 * @file view_settings_tabs.cpp
 * @brief Settings tab bar.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of TupoyeMenu.
 * TupoyeMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * TupoyeMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with TupoyeMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "lua/lua_manager.hpp"
#include "views/view.hpp"

namespace big
{
	void view::settings_tabs()
	{
		if (ImGui::BeginTabBar("settings_tabbar"))
		{
			if (ImGui::BeginTabItem("Settings"))
			{
				view::settings();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_SETTINGS"));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Hotkeys"))
			{
				view::hotkey_settings();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_HOTKEY_SETTINGS"));
				ImGui::EndTabItem();
			}
#if defined (ENABLE_LUA)
			if (ImGui::BeginTabItem("Lua Scripts"))
			{
				view::lua_scripts();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_LUA_SCRIPTS"));
				ImGui::EndTabItem();
			}
#endif // ENABLE_LUA
			if (ImGui::BeginTabItem("Context Menu"))
			{
				view::context_menu_settings();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_CONTEXT_MENU_SETTINGS"));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("ESP"))
			{
				view::esp_settings();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_ESP_SETTINGS"));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("GUI"))
			{
				view::gui_settings();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_GUI_SETTINGS"));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Reactions"))
			{
				view::reaction_settings();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_REACTION_SETTINGS"));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Protection"))
			{
				view::protection_settings();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_PROTECTION_SETTINGS"));
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("GTA Cache"))
			{
				view::gta_cache();
				if (g_lua_manager)
					g_lua_manager->draw_gui(RAGE_JOAAT("GUI_TAB_GTA_CACHE_SETTINGS"));
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
}
