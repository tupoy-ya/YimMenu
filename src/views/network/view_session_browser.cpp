/**
 * @file view_session_browser.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "core/data/language_codes.hpp"
#include "core/data/region_codes.hpp"
#include "fiber_pool.hpp"
#include "pointers.hpp"
#include "script.hpp"
#include "services/matchmaking/matchmaking_service.hpp"
#include "util/session.hpp"
#include "views/view.hpp"

#include <network/Network.hpp>

namespace big
{
	static int selected_session_idx = -1;

	void view::session_browser()
	{
		static char name_buf[32];
		static char search[64];
		static char session_info[255];

		ImGui::Text(std::format("Total sessions found: {}", g_matchmaking_service->get_num_found_sessions()).data());

		ImGui::SetNextItemWidth(300.f);

		if (ImGui::ListBoxHeader("###sessions", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y - 400 - 38 * 4)}))
		{
			if (g_matchmaking_service->get_num_found_sessions())
			{
				for (int i = 0; i < g_matchmaking_service->get_num_found_sessions(); i++)
				{
					auto& session = g_matchmaking_service->get_found_sessions()[i];

					if (!session.is_valid)
						continue;

					if (components::selectable(std::to_string(session.info.m_session_token), i == selected_session_idx))
					{
						selected_session_idx = i;
						g_pointers->m_gta.m_encode_session_info(&session.info, session_info, 0x7D, nullptr);
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::SetTooltip(std::format("Num Players: {}\nRegion: {}\nLanguage: {}\nHost: {}",
						    session.attributes.player_count,
						    regions[session.attributes.region].name,
						    languages[session.attributes.language].name,
						    session.info.m_net_player_data.m_gamer_handle.m_rockstar_id)
						                      .c_str());
					}
				}
			}
			else
			{
				ImGui::Text("No sessions");
			}

			ImGui::ListBoxFooter();
		}

		if (selected_session_idx != -1)
		{
			ImGui::SameLine();
			if (ImGui::BeginChild("###selected_session", {300, static_cast<float>(*g_pointers->m_gta.m_resolution_y - 388 - 38 * 4)}, false, ImGuiWindowFlags_NoBackground))
			{
				auto& session = g_matchmaking_service->get_found_sessions()[selected_session_idx];

				ImGui::Text("Num Players: %d", session.attributes.player_count);
				ImGui::Text("Discriminator: 0x%X", session.attributes.discriminator);
				ImGui::Text("Region: %s", regions[session.attributes.region].name);
				ImGui::Text("Language: %s", languages[session.attributes.language].name);

				auto& data = session.info.m_net_player_data;
				ImGui::Text("Host Rockstar ID: %d", data.m_gamer_handle.m_rockstar_id);

				components::button("Copy Session Info", [] {
					ImGui::SetClipboardText(session_info);
				});
				ImGui::SameLine();
				components::button("Join", [session] {
					if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(RAGE_JOAAT("maintransition")) != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
					{
						g_notification_service->push_error("Join Session", "Player switch in progress, wait a bit.");
						return;
					}

					bool is_session_free_aim = session.attributes.discriminator & (1 << 17);
					bool is_local_free_aim   = PAD::GET_LOCAL_PLAYER_GAMEPAD_AIM_STATE() > 1;

					if (is_session_free_aim != is_local_free_aim)
						PLAYER::SET_PLAYER_TARGETING_MODE(is_session_free_aim ? 3 : 1);

					session::join_session(session.info);
				});
			}
			ImGui::EndChild();
		}

		if (ImGui::TreeNode("Filters"))
		{
			ImGui::Checkbox("Region", &g.session_browser.region_filter_enabled);
			ImGui::SameLine();
			components::help_marker("It is highly recommended to keep this filter enabled");

			if (g.session_browser.region_filter_enabled)
			{
				ImGui::SameLine();

				if (ImGui::BeginCombo("###region_select", regions[g.session_browser.region_filter].name))
				{
					for (const auto& region : regions)
					{
						if (ImGui::Selectable(region.name, g.session_browser.region_filter == region.id))
						{
							g.session_browser.region_filter = region.id;
						}
					}
					ImGui::EndCombo();
				}
			}

			ImGui::Checkbox("Language", &g.session_browser.language_filter_enabled);
			ImGui::SameLine();
			components::help_marker("Setting a correct region filter for the language will help tremendously");

			if (g.session_browser.language_filter_enabled)
			{
				ImGui::SameLine();

				if (ImGui::BeginCombo("###language_select", languages[g.session_browser.language_filter].name))
				{
					for (const auto& language : languages)
					{
						if (ImGui::Selectable(language.name, g.session_browser.language_filter == language.id))
						{
							g.session_browser.language_filter = language.id;
						};
					}
					ImGui::EndCombo();
				}
			}

			ImGui::Checkbox("Players", &g.session_browser.player_count_filter_enabled);

			if (g.session_browser.player_count_filter_enabled)
			{
				ImGui::InputInt("Minimum", &g.session_browser.player_count_filter_minimum);
				ImGui::InputInt("Maximum", &g.session_browser.player_count_filter_maximum);
			}

			ImGui::Checkbox("Pool Type", &g.session_browser.pool_filter_enabled);
			if (g.session_browser.pool_filter_enabled)
			{
				ImGui::SameLine();
				ImGui::Combo("###pooltype", &g.session_browser.pool_filter, "Normal\0Bad Sport");
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Sorting"))
		{
			ImGui::Combo("Sort By", &g.session_browser.sort_method, "Off\0Player Count");
			if (g.session_browser.sort_method != 0)
				ImGui::Combo("Direction", &g.session_browser.sort_direction, "Ascending\0Descending");
			ImGui::TreePop();
		}

		if (ImGui::Checkbox("Replace Game Matchmaking", &g.session_browser.replace_game_matchmaking))
			;
		ImGui::SameLine();
		components::help_marker("This will replace the default game matchmaking with a custom one that will use the filters and sorting set here");

		components::button("Refresh", [] {
			selected_session_idx = -1;

			if (!g_matchmaking_service->matchmake())
				g_notification_service->push_error("Matchmaking", "Matchmaking failed");
		});
	}
}
