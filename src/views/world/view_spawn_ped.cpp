/**
 * @file view_spawn_ped.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "backend/looped/looped.hpp"
#include "fiber_pool.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "script.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/model_preview/model_preview_service.hpp"
#include "services/players/player_service.hpp"
#include "util/ped.hpp"
#include "views/view.hpp"

#include <imgui_internal.h>


#define SPAWN_PED_ALL_WEAPONS -1
#define SPAWN_PED_NO_WEAPONS -2

#define SPAWN_PED_FOR_SELF -1
#define SPAWN_PED_FOR_EVERYONE -2

static int selected_ped_weapon_type  = SPAWN_PED_ALL_WEAPONS;
static Hash selected_ped_weapon_hash = 0;

namespace big
{
	void spawn_ped_give_weapon(const Ped ped)
	{
		if (selected_ped_weapon_type == SPAWN_PED_NO_WEAPONS)
		{
			return;
		}

		const auto& weapon_type_arr = g_gta_data_service->weapon_types();
		for (auto& [_, weapon] : g_gta_data_service->weapons())
		{
			if (selected_ped_weapon_type == SPAWN_PED_ALL_WEAPONS || weapon.m_weapon_type == weapon_type_arr[selected_ped_weapon_type])
			{
				if ((selected_ped_weapon_hash == 0 || weapon.m_hash == selected_ped_weapon_hash) && weapon.m_hash != RAGE_JOAAT("WEAPON_UNARMED"))
				{
					WEAPON::GIVE_WEAPON_TO_PED(ped, weapon.m_hash, 9999, false, selected_ped_weapon_hash != 0);
				}
			}
		}
	}

	Ped spawn_ped_at_location(const int selected_ped_type, const char* ped_model_buf, const Player selected_ped_player_id, const Player selected_ped_for_player_id, const bool is_bodyguard)
	{
		Hash hash = 0;
		Ped clone = 0;
		Vector3 location;
		Player player;
		Ped player_ped;

		if (selected_ped_type == -2)
		{
			if (selected_ped_player_id == -1)
			{
				clone = self::ped;
				hash  = ENTITY::GET_ENTITY_MODEL(clone);
			}
			else
			{
				auto plyr = g_player_service->get_by_id(selected_ped_player_id);
				if (plyr == nullptr || !plyr->is_valid() || !plyr->get_ped() || !plyr->get_ped()->m_navigation)
				{
					g_notification_service->push_error("Ped", "Invalid Online Player.");
					return 0;
				}

				clone = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(plyr->id());
				hash  = ENTITY::GET_ENTITY_MODEL(clone);
			}
		}
		else
		{
			hash = rage::joaat(ped_model_buf);
		}


		if (selected_ped_for_player_id == SPAWN_PED_FOR_SELF)
		{
			location   = self::pos;
			player     = self::id;
			player_ped = self::ped;
		}
		else
		{
			auto plyr = g_player_service->get_by_id(selected_ped_for_player_id);
			if (plyr == nullptr || !plyr->is_valid() || !plyr->get_ped() || !plyr->get_ped()->m_navigation)
			{
				g_notification_service->push_error("Ped", "Invalid Online Player.");
				return 0;
			}

			auto player_pos = *plyr->get_ped()->m_navigation->get_position();

			location.x = player_pos.x;
			location.y = player_pos.y;
			location.z = player_pos.z;
			player     = plyr->id();
			player_ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
		}

		location.x += 1.f;
		location.y += 1.f;
		Ped ped = ped::spawn(ePedType::PED_TYPE_ARMY, hash, clone, location, 0);

		if (ped == 0)
		{
			g_notification_service->push_error("Ped", "Failed to spawn model, did you give an incorrect model ?");
			return 0;
		}

		PED::SET_PED_ARMOUR(ped, 100);
		ENTITY::SET_ENTITY_MAX_HEALTH(ped, 1000);
		ENTITY::SET_ENTITY_HEALTH(ped, 1000, 0, 0);
		PED::SET_PED_COMBAT_ABILITY(ped, 100);
		PED::SET_PED_ACCURACY(ped, 100);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 1, 1);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 3, 1);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 5, 1);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 13, 1);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 21, 1);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 27, 1);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 41, 1);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 46, 1);
		PED::SET_PED_COMBAT_ATTRIBUTES(ped, 63, 0);
		PED::SET_PED_COMBAT_ABILITY(ped, 2);
		PED::SET_PED_COMBAT_MOVEMENT(ped, 2);
		PED::SET_PED_COMBAT_RANGE(ped, 0);
		PED::SET_PED_HIGHLY_PERCEPTIVE(ped, true);
		PED::SET_PED_SEEING_RANGE(ped, 200.0f);
		PED::SET_PED_HEARING_RANGE(ped, 200.0f);
		PED::SET_PED_ID_RANGE(ped, 200.0f);
		PED::SET_PED_FIRING_PATTERN(ped, RAGE_JOAAT("FIRING_PATTERN_FULL_AUTO"));
		PED::SET_PED_SHOOT_RATE(ped, 150);
		ped::set_ped_random_component_variation(ped);

		if (is_bodyguard)
		{
			int player_group = PED::GET_PED_GROUP_INDEX(player_ped);

			if (!PED::DOES_GROUP_EXIST(player_group))
				player_group = PED::CREATE_GROUP(0);

			PED::SET_PED_AS_GROUP_LEADER(player_ped, player_group);
			PED::SET_PED_AS_GROUP_MEMBER(ped, player_group);
			PED::SET_PED_CAN_TELEPORT_TO_GROUP_LEADER(ped, player_group, true);
			PED::SET_PED_NEVER_LEAVES_GROUP(ped, true);
			PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, PED::GET_PED_RELATIONSHIP_GROUP_HASH(player_ped));
			PED::SET_PED_CAN_BE_TARGETTED_BY_PLAYER(ped, player, true);

			if (player != self::id)
			{
				PED::SET_PED_KEEP_TASK(ped, true);
				PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
				TASK::TASK_SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
				TASK::TASK_FOLLOW_TO_OFFSET_OF_ENTITY(ped, player, 0.0f, 0.0f, 0.0f, 4.0f, -1, 0.0f, true);
			}
		}

		if (g.world.spawn_ped.spawn_invincible)
		{
			ENTITY::SET_ENTITY_INVINCIBLE(ped, true);
		}

		if (g.world.spawn_ped.spawn_invisible)
		{
			ENTITY::SET_ENTITY_VISIBLE(ped, false, false);
		}

		if (g.world.spawn_ped.spawn_as_attacker)
		{
			PED::SET_PED_AS_ENEMY(ped, true);
			PED::SET_PED_KEEP_TASK(ped, true);
			PED::SET_BLOCKING_OF_NON_TEMPORARY_EVENTS(ped, true);
			TASK::TASK_COMBAT_PED(ped, player_ped, 0, 16);
			PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, RAGE_JOAAT("HATES_PLAYER"));
			PED::SET_PED_ALERTNESS(ped, 3);
		}

		spawn_ped_give_weapon(ped);
		spawned_peds.push_back({ped,
		    selected_ped_for_player_id == SPAWN_PED_FOR_SELF ? self::id : selected_ped_for_player_id,
		    is_bodyguard,
		    g.world.spawn_ped.spawn_as_attacker});
		return ped;
	}

	void view::spawn_ped()
	{
		static int selected_ped_type        = -2;
		static bool ped_model_dropdown_open = false;
		static char ped_model_buf[64];
		static Player selected_ped_player_id = -1;

		auto& ped_type_arr = g_gta_data_service->ped_types();
		auto& ped_arr      = g_gta_data_service->peds();

		auto& weapon_type_arr = g_gta_data_service->weapon_types();
		auto& weapon_arr      = g_gta_data_service->weapons();

		static Player selected_ped_for_player_id = -1;
		auto& player_arr                         = g_player_service->players();

		if (!*g_pointers->m_gta.m_is_session_started)
		{
			selected_ped_player_id = -1;

			if (selected_ped_for_player_id != SPAWN_PED_FOR_SELF && selected_ped_for_player_id != SPAWN_PED_FOR_EVERYONE)
				selected_ped_for_player_id = SPAWN_PED_FOR_SELF;
		}
		else
		{
			if (g_player_service->get_by_id(selected_ped_player_id) == nullptr)
			{
				selected_ped_player_id = -1;
			}

			if (selected_ped_for_player_id != SPAWN_PED_FOR_SELF && selected_ped_for_player_id != SPAWN_PED_FOR_EVERYONE
			    && g_player_service->get_by_id(selected_ped_for_player_id) == nullptr)
			{
				selected_ped_for_player_id = SPAWN_PED_FOR_SELF;
			}
		}

		ImGui::TextUnformatted("Ped Model");
		{
			ImGui::BeginGroup();
			{
				ImGui::TextUnformatted("Ped Type");

				ImGui::SetNextItemWidth(160.f);
				if (ImGui::BeginCombo("##ped_type",
				        selected_ped_type == -1     ? "All" :
				            selected_ped_type == -2 ? "ONLINE PLAYER" :
				                                      ped_type_arr[selected_ped_type].c_str()))
				{
					if (ImGui::Selectable("ONLINE PLAYER", selected_ped_type == -2))
					{
						selected_ped_type = -2;
					}

					if (ImGui::Selectable("All", selected_ped_type == -1))
					{
						selected_ped_type = -1;
					}

					for (int i = 0; i < ped_type_arr.size(); i++)
					{
						if (ImGui::Selectable(ped_type_arr[i].c_str(), selected_ped_type == i))
						{
							selected_ped_type = i;
							ped_model_buf[0]  = 0;
						}

						if (selected_ped_type == i)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}
			}
			ImGui::EndGroup();

			ImGui::SameLine();

			if (selected_ped_type == -2)
			{
				ImGui::BeginGroup();
				{
					ImGui::TextUnformatted("Player");

					ImGui::SetNextItemWidth(240.f);
					if (ImGui::BeginCombo("##ped_player",
					        selected_ped_player_id == -1 ? "Self" : g_player_service->get_by_id(selected_ped_player_id)->get_name()))
					{
						if (ImGui::Selectable("Self", selected_ped_player_id == -1))
						{
							selected_ped_player_id = -1;
							g_model_preview_service->stop_preview();
						}
						else if (!g.world.spawn_ped.preview_ped || (g.world.spawn_ped.preview_ped && !ImGui::IsAnyItemHovered()))
						{
							g_model_preview_service->stop_preview();
						}
						else if (ImGui::IsItemHovered())
						{
							g_fiber_pool->queue_job([] {
								Ped ped   = self::ped;
								Hash hash = ENTITY::GET_ENTITY_MODEL(ped);
								g_model_preview_service->show_ped(hash, ped);
							});
						}

						if (selected_ped_player_id == -1)
						{
							ImGui::SetItemDefaultFocus();
						}

						if (*g_pointers->m_gta.m_is_session_started)
						{
							for (auto& item : player_arr)
							{
								auto plyr    = item.second;
								auto plyr_id = plyr->id();

								ImGui::PushID(plyr_id);
								if (ImGui::Selectable(plyr->get_name(), selected_ped_player_id == plyr_id))
								{
									selected_ped_player_id = plyr_id;
									g_model_preview_service->stop_preview();
								}
								else if (!g.world.spawn_ped.preview_ped || (g.world.spawn_ped.preview_ped && !ImGui::IsAnyItemHovered()))
								{
									g_model_preview_service->stop_preview();
								}
								else if (ImGui::IsItemHovered())
								{
									g_fiber_pool->queue_job([plyr_id] {
										auto plyr = g_player_service->get_by_id(plyr_id);
										if (plyr)
										{
											Ped ped   = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(plyr->id());
											Hash hash = ENTITY::GET_ENTITY_MODEL(ped);
											g_model_preview_service->show_ped(hash, ped);
										}
									});
								}
								ImGui::PopID();

								if (selected_ped_player_id == plyr_id)
								{
									ImGui::SetItemDefaultFocus();
								}
							}
						}

						ImGui::EndCombo();
					}
				}
				ImGui::EndGroup();
			}
			else
			{
				ImGui::BeginGroup();
				{
					ImGui::TextUnformatted("Model Name");

					ImGui::SetNextItemWidth(240.f);
					components::input_text_with_hint("##ped_model_name", "Model Name", ped_model_buf, sizeof(ped_model_buf), ImGuiInputTextFlags_EnterReturnsTrue, [] {
						ped_model_dropdown_open = false;
					});
				}
				ImGui::EndGroup();

				// ped model dropdown
				{
					bool ped_model_dropdown_focused = ImGui::IsItemActive();

					if (ImGui::IsItemActivated())
					{
						ped_model_dropdown_open = true;
					}

					if (ped_model_dropdown_open)
					{
						bool is_open      = true;
						bool item_hovered = false;

						std::string lower_search = ped_model_buf;
						std::transform(lower_search.begin(), lower_search.end(), lower_search.begin(), tolower);

						ImGui::SetNextWindowPos({ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y});
						ImGui::SetNextWindowSize({300, 300});
						if (ImGui::Begin("##player_model_popup", &is_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_Tooltip))
						{
							ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
							ped_model_dropdown_focused |= ImGui::IsWindowFocused();

							for (const auto& [_, item] : ped_arr)
							{
								std::string ped_type = item.m_ped_type;
								std::string name     = item.m_name;

								std::transform(name.begin(), name.end(), name.begin(), tolower);

								if ((selected_ped_type == -1 || ped_type_arr[selected_ped_type] == ped_type) && (name.find(lower_search) != std::string::npos))
								{
									bool selectable_highlighted = lower_search == name;
									bool selectable_clicked = ImGui::Selectable(item.m_name, selectable_highlighted);
									ped_model_dropdown_focused |= ImGui::IsItemFocused();

									if (selectable_clicked)
									{
										strncpy(ped_model_buf, item.m_name, 64);
										ped_model_dropdown_open    = false;
										ped_model_dropdown_focused = false;
									}

									if (selectable_highlighted)
									{
										ImGui::SetItemDefaultFocus();
									}

									if (ImGui::IsItemHovered())
									{
										item_hovered = true;
										g_model_preview_service->show_ped(item.m_hash);
									}
								}
							}
							ImGui::End();
						}

						ped_model_dropdown_open = ped_model_dropdown_focused;

						if (!g.world.spawn_ped.preview_ped || (g.world.spawn_ped.preview_ped && (!item_hovered || !ped_model_dropdown_open)))
						{
							g_model_preview_service->stop_preview();
						}
					}
				}
			}
		}

		ImGui::SeparatorText("Weapon");
		{
			ImGui::BeginGroup();
			{
				ImGui::TextUnformatted("Weapon Type");

				ImGui::SetNextItemWidth(160.f);
				if (ImGui::BeginCombo("##ped_weapon_type",
				        selected_ped_weapon_type == SPAWN_PED_ALL_WEAPONS    ? "All" :
				            selected_ped_weapon_type == SPAWN_PED_NO_WEAPONS ? "NO WEAPONS" :
				                                                               weapon_type_arr[selected_ped_weapon_type].c_str()))
				{
					if (ImGui::Selectable("All", selected_ped_weapon_type == SPAWN_PED_ALL_WEAPONS))
					{
						selected_ped_weapon_type = SPAWN_PED_ALL_WEAPONS;
					}

					if (selected_ped_weapon_hash == SPAWN_PED_ALL_WEAPONS)
					{
						ImGui::SetItemDefaultFocus();
					}

					if (ImGui::Selectable("NO WEAPONS", selected_ped_weapon_type == SPAWN_PED_NO_WEAPONS))
					{
						selected_ped_weapon_type = SPAWN_PED_NO_WEAPONS;
					}

					if (selected_ped_weapon_hash == SPAWN_PED_NO_WEAPONS)
					{
						ImGui::SetItemDefaultFocus();
					}

					for (int i = 0; i < weapon_type_arr.size(); i++)
					{
						if (ImGui::Selectable(weapon_type_arr[i].c_str(), selected_ped_weapon_type == i))
						{
							selected_ped_weapon_type = i;
							selected_ped_weapon_hash = 0;
						}

						if (selected_ped_weapon_type == i)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}
			}
			ImGui::EndGroup();

			ImGui::SameLine();

			ImGui::BeginGroup();
			{
				ImGui::TextUnformatted("Weapon");

				ImGui::SetNextItemWidth(240.f);
				if (ImGui::BeginCombo("##ped_weapon",
				        selected_ped_weapon_type == SPAWN_PED_NO_WEAPONS ?
				            "NO WEAPONS" :
				            selected_ped_weapon_hash == 0 ?
				            "All" :
				            g_gta_data_service->weapon_by_hash(selected_ped_weapon_hash).m_display_name.c_str()))
				{
					if (selected_ped_weapon_type != SPAWN_PED_NO_WEAPONS)
					{
						if (ImGui::Selectable("All", selected_ped_weapon_hash == 0))
						{
							selected_ped_weapon_hash = 0;
						}

						if (selected_ped_weapon_hash == 0)
						{
							ImGui::SetItemDefaultFocus();
						}

						for (const auto& [_, weapon] : weapon_arr)
						{
							if (selected_ped_weapon_type == SPAWN_PED_ALL_WEAPONS || weapon.m_weapon_type == weapon_type_arr[selected_ped_weapon_type])
							{
								if (ImGui::Selectable(weapon.m_display_name.c_str(), weapon.m_hash == selected_ped_weapon_hash))
								{
									selected_ped_weapon_hash = weapon.m_hash;
								}
							}

							if (selected_ped_weapon_hash == weapon.m_hash)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
					}

					ImGui::EndCombo();
				}
			}
			ImGui::EndGroup();
		}

		ImGui::SeparatorText("Spawn For");
		{
			if (ImGui::BeginCombo("##ped_for",
			        (selected_ped_for_player_id == SPAWN_PED_FOR_SELF ?
			                "Self" :
			                (selected_ped_for_player_id == SPAWN_PED_FOR_EVERYONE ?
			                        "Everyone" :
			                        g_player_service->get_by_id(selected_ped_for_player_id)->get_name()))))
			{
				if (ImGui::Selectable("Self", selected_ped_for_player_id == SPAWN_PED_FOR_SELF))
				{
					selected_ped_for_player_id = SPAWN_PED_FOR_SELF;
				}

				if (selected_ped_for_player_id == SPAWN_PED_FOR_SELF)
				{
					ImGui::SetItemDefaultFocus();
				}

				if (ImGui::Selectable("Everyone", selected_ped_for_player_id == SPAWN_PED_FOR_EVERYONE))
				{
					selected_ped_for_player_id = SPAWN_PED_FOR_EVERYONE;
				}

				if (selected_ped_for_player_id == SPAWN_PED_FOR_EVERYONE)
				{
					ImGui::SetItemDefaultFocus();
				}

				if (*g_pointers->m_gta.m_is_session_started)
				{
					for (auto& [_, plyr] : player_arr)
					{
						auto plyr_id = plyr->id();

						ImGui::PushID(plyr_id);
						if (ImGui::Selectable(plyr->get_name(), selected_ped_for_player_id == plyr_id))
						{
							selected_ped_for_player_id = plyr_id;
						}
						ImGui::PopID();

						if (selected_ped_for_player_id == plyr_id)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}

				ImGui::EndCombo();
			}
		}
		ImGui::Separator();


		if (ImGui::Checkbox("Preview", &g.world.spawn_ped.preview_ped))
		{
			if (!g.world.spawn_ped.preview_ped)
			{
				g_model_preview_service->stop_preview();
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", "Shows a render of what you have highlighted in front of you.");

		ImGui::Checkbox("Invincible", &g.world.spawn_ped.spawn_invincible);
		ImGui::Checkbox("Invisible", &g.world.spawn_ped.spawn_invisible);
		ImGui::Checkbox("Attacker", &g.world.spawn_ped.spawn_as_attacker);

		components::button("Change Player Model", [] {
			if (selected_ped_type == -2)
			{
				if (selected_ped_player_id != -1)
				{
					auto plyr = g_player_service->get_by_id(selected_ped_player_id);
					if (plyr)
					{
						Ped ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(plyr->id());
						ped::steal_identity(ped);
					}
				}
			}
			else
			{
				if (!ped::change_player_model(rage::joaat(ped_model_buf)))
				{
					g_notification_service->push_error("Ped", "Failed to spawn model, did you give an incorrect model ?");
					return;
				}

				ped::set_ped_random_component_variation(self::ped);
			}
		});

		ImGui::SameLine();


		components::button("Spawn Ped", [] {
			if (selected_ped_for_player_id == SPAWN_PED_FOR_EVERYONE)
			{
				g_player_service->iterate([](const big::player_entry& entry) {
					spawn_ped_at_location(selected_ped_type, ped_model_buf, selected_ped_player_id, entry.second->id(), false);
				});
			}
			else
			{
				spawn_ped_at_location(selected_ped_type, ped_model_buf, selected_ped_player_id, selected_ped_for_player_id, false);
			}
		});

		ImGui::SameLine();

		components::button("Spawn Bodyguard", [] {
			if (selected_ped_for_player_id == SPAWN_PED_FOR_EVERYONE)
			{
				g_player_service->iterate([](const big::player_entry& entry) {
					spawn_ped_at_location(selected_ped_type, ped_model_buf, selected_ped_player_id, entry.second->id(), true);
				});
			}
			else
			{
				spawn_ped_at_location(selected_ped_type, ped_model_buf, selected_ped_player_id, selected_ped_for_player_id, true);
			}
		});

		components::button("Spoof As Model", [] {
			g.spoofing.spoof_player_model = true;
			g.spoofing.player_model       = ped_model_buf;
		});

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("%s", "This WILL break freemode missions and jobs!");

		if (g.spoofing.spoof_player_model)
		{
			ImGui::SameLine();
			components::button("Unspoof Model", [] {
				g.spoofing.spoof_player_model = false;
			});
		}

		components::button("Cleanup Spawned Peds", [] {
			for (auto& ped : spawned_peds)
			{
				PED::DELETE_PED(&ped.ped_handle);
			}

			spawned_peds.clear();
		});
	}
}
