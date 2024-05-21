#include "core/data/bullet_impact_types.hpp"
#include "core/data/special_ammo_types.hpp"
#include "fiber_pool.hpp"
#include "gta/joaat.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "services/persist_weapons/persist_weapons.hpp"
#include "views/view.hpp"

namespace big
{
	struct custom_weapon
	{
		big::CustomWeapon id;
		const char* name;
	};

	const custom_weapon custom_weapons[] = {
	    {big::CustomWeapon::NONE, "No Weapon"},
	    {big::CustomWeapon::CAGE_GUN, "Cage Gun"},
	    {big::CustomWeapon::DELETE_GUN, "Delete Gun"},
	    {big::CustomWeapon::GRAVITY_GUN, "Gravity Gun"},
	    {big::CustomWeapon::STEAL_VEHICLE_GUN, "Steal Vehicle Gun"},
	    {big::CustomWeapon::REPAIR_GUN, "Repair Gun"},
	    {big::CustomWeapon::VEHICLE_GUN, "Vehicle Gun"},
	    {big::CustomWeapon::TP_GUN, "Teleporter Gun"},
	    {big::CustomWeapon::PAINT_GUN, "Paint Gun"},
	};

	void view::weapons()
	{
		ImGui::TextUnformatted("Ammo");

		ImGui::BeginGroup();
		components::command_checkbox<"infammo">();
		components::command_checkbox<"infrange">();
		ImGui::Checkbox("Increase C4 Limit (Max = 50)", &g.weapons.increased_c4_limit);
		components::command_checkbox<"allowwepsinside">();
		components::command_checkbox<"rapidfire">();
		components::command_checkbox<"enablemk1variants">();

		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();

		components::command_checkbox<"alwaysfullammo">();
		components::command_checkbox<"infclip">();
		ImGui::Checkbox("Increase Flare Limit (Max = 50)", &g.weapons.increased_flare_limit);
		components::command_checkbox<"vehallweapons">();
		components::command_checkbox<"infinitevehammo">();

		ImGui::Checkbox("Enable Special Ammo", &g.weapons.ammo_special.toggle);
		components::options_modal("Special ammo", [] {
			eAmmoSpecialType selected_ammo   = g.weapons.ammo_special.type;
			eExplosionTag selected_explosion = g.weapons.ammo_special.explosion_tag;

			if (ImGui::BeginCombo("Special Ammo", SPECIAL_AMMOS[(int)selected_ammo].name))
			{
				for (const auto& special_ammo : SPECIAL_AMMOS)
				{
					if (ImGui::Selectable(special_ammo.name, special_ammo.type == selected_ammo))
					{
						g.weapons.ammo_special.type = special_ammo.type;
					}

					if (special_ammo.type == selected_ammo)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::BeginCombo("Bullet Impact", BULLET_IMPACTS[selected_explosion]))
			{
				for (const auto& [type, name] : BULLET_IMPACTS)
				{
					if (ImGui::Selectable(name, type == selected_explosion))
					{
						g.weapons.ammo_special.explosion_tag = type;
					}

					if (type == selected_explosion)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		});

		ImGui::EndGroup();

		ImGui::SeparatorText("Misc");

		components::command_checkbox<"norecoil">();
		ImGui::SameLine();
		components::command_checkbox<"nospread">();

		components::button("Get All Weapons", [] {
			for (const auto& [_, weapon] : g_gta_data_service->weapons())
			{
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(self::ped, weapon.m_hash, 9999, false);
			}

			constexpr auto parachute_hash = "GADGET_PARACHUTE"_J;
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(self::ped, parachute_hash, 0, true);
		});
		ImGui::SameLine();
		components::button("Remove Current Weapon", [] {
			Hash weaponHash;
			WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weaponHash, 1);
			if (weaponHash != "WEAPON_UNARMED"_J)
			{
				WEAPON::REMOVE_WEAPON_FROM_PED(self::ped, weaponHash);
			}
		});

		components::command_checkbox<"incrdamage">();
		ImGui::InputFloat("Damage", &g.weapons.increased_damage, .1, 10, "%.1f");


		components::command_checkbox<"modifyexplosionradius">();
		ImGui::InputFloat("Explosion Radius", &g.weapons.set_explosion_radius, .1, 200, "%.1f");

		ImGui::SeparatorText("Custom Weapons");

		ImGui::Checkbox("Custom Gun only fires when the weapon is out", &g.self.custom_weapon_stop);
		CustomWeapon selected = g.weapons.custom_weapon;

		if (ImGui::BeginCombo("Weapon", custom_weapons[(int)selected].name))
		{
			for (const custom_weapon& weapon : custom_weapons)
			{
				if (ImGui::Selectable(weapon.name, weapon.id == selected))
				{
					g.weapons.custom_weapon = weapon.id;
				}

				if (weapon.id == selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		switch (selected)
		{
		case CustomWeapon::GRAVITY_GUN:
			ImGui::Checkbox("Launch on release", &g.weapons.gravity_gun.launch_on_release);
			break;
		case CustomWeapon::VEHICLE_GUN:
			// this some ugly ass looking code
			static char vehicle_gun[12];
			std::memcpy(vehicle_gun, g.weapons.vehicle_gun_model.c_str(), 12);
			if (ImGui::InputTextWithHint("Shooting Model", "Name of the vehicle model", vehicle_gun, sizeof(vehicle_gun)))
			{
				g.weapons.vehicle_gun_model = vehicle_gun;
			}
			if (ImGui::IsItemActive())
				g.self.hud.typing = TYPING_TICKS;

			break;
		case CustomWeapon::PAINT_GUN:
			ImGui::Checkbox("Rainbow Color", &g.weapons.paintgun.rainbow);
			ImGui::SliderFloat("Rainbow Speed", &g.weapons.paintgun.speed, 0.f, 10.f);
			if (!g.weapons.paintgun.rainbow)
			{
				ImGui::ColorEdit4("Paint Gun Color", g.weapons.paintgun.col);
			}
		default: break;
		}

		ImGui::Separator();

		if (ImGui::TreeNode("Ammunation"))
		{
			static Hash selected_weapon_hash, selected_weapon_attachment_hash{};
			static std::string selected_weapon, selected_weapon_attachment;
			ImGui::PushItemWidth(300);
			if (ImGui::BeginCombo("Weapons", selected_weapon.c_str()))
			{
				std::map<std::string, weapon_item> sorted_map;
				for (const auto& [_, weapon] : g_gta_data_service->weapons())
				{
					sorted_map.emplace(weapon.m_display_name, weapon);
				}
				for (const auto& weapon : sorted_map)
				{
					bool is_selected = weapon.second.m_hash == selected_weapon_hash;
					if (weapon.second.m_display_name != "NULL" && ImGui::Selectable(weapon.second.m_display_name.c_str(), is_selected, ImGuiSelectableFlags_None))
					{
						selected_weapon                 = weapon.second.m_display_name;
						selected_weapon_hash            = weapon.second.m_hash;
						selected_weapon_attachment_hash = {};
						selected_weapon_attachment.clear();
					}
					if (is_selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();
			components::button("Give Weapon", [] {
				WEAPON::GIVE_WEAPON_TO_PED(self::ped, selected_weapon_hash, 9999, false, true);
			});
			ImGui::SameLine();
			components::button("Remove Weapon", [] {
				WEAPON::REMOVE_WEAPON_FROM_PED(self::ped, selected_weapon_hash);
			});

			ImGui::PushItemWidth(250);
			if (ImGui::BeginCombo("Attachments", selected_weapon_attachment.c_str()))
			{
				weapon_item weapon = g_gta_data_service->weapon_by_hash(selected_weapon_hash);
				if (!weapon.m_attachments.empty())
				{
					for (std::string attachment : weapon.m_attachments)
					{
						weapon_component attachment_component = g_gta_data_service->weapon_component_by_name(attachment);
						std::string attachment_name = attachment_component.m_display_name;
						Hash attachment_hash        = attachment_component.m_hash;
						if (attachment_hash == 0)
						{
							attachment_name = attachment;
							attachment_hash = rage::joaat(attachment);
						}
						bool is_selected         = attachment_hash == selected_weapon_attachment_hash;
						std::string display_name = attachment_name.append("##").append(std::to_string(attachment_hash));
						if (ImGui::Selectable(display_name.c_str(), is_selected, ImGuiSelectableFlags_None))
						{
							selected_weapon_attachment      = attachment_name;
							selected_weapon_attachment_hash = attachment_hash;
						}
						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
				}

				ImGui::EndCombo();
			}
			ImGui::SameLine();
			components::button("Add to Weapon", [] {
				WEAPON::GIVE_WEAPON_COMPONENT_TO_PED(self::ped, selected_weapon_hash, selected_weapon_attachment_hash);
			});
			ImGui::SameLine();
			components::button("Remove from Weapon", [] {
				WEAPON::REMOVE_WEAPON_COMPONENT_FROM_PED(self::ped, selected_weapon_hash, selected_weapon_attachment_hash);
			});
			ImGui::PopItemWidth();

			static const char* default_tints[]{"Black tint", "Green tint", "Gold tint", "Pink tint", "Army tint", "LSPD tint", "Orange tint", "Platinum tint"};
			static const char* mk2_tints[]{"Classic Black", "Classic Grey", "Classic Two - Tone", "Classic White", "Classic Beige", "Classic Green", "Classic Blue", "Classic Earth", "Classic Brown & Black", "Red Contrast", "Blue Contrast", "Yellow Contrast", "Orange Contrast", "Bold Pink", "Bold Purple & Yellow", "Bold Orange", "Bold Green & Purple", "Bold Red Features", "Bold Green Features", "Bold Cyan Features", "Bold Yellow Features", "Bold Red & White", "Bold Blue & White", "Metallic Gold", "Metallic Platinum", "Metallic Grey & Lilac", "Metallic Purple & Lime", "Metallic Red", "Metallic Green", "Metallic Blue", "Metallic White & Aqua", "Metallic Red & Yellow"};
			static int tint;

			if (selected_weapon.ends_with("Mk II"))
			{
				ImGui::Combo("Tints", &tint, mk2_tints, IM_ARRAYSIZE(mk2_tints));
			}
			else
			{
				ImGui::Combo("Tints", &tint, default_tints, IM_ARRAYSIZE(default_tints));
			}
			ImGui::SameLine();
			components::button("Apply", [] {
				WEAPON::SET_PED_WEAPON_TINT_INDEX(self::ped, selected_weapon_hash, tint);
			});
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Persist Weapons"))
		{
			ImGui::PushID(1);
			ImGui::Checkbox("Enabled", &g.persist_weapons.enabled);
			ImGui::PopID();

			static std::string selected_loadout = g.persist_weapons.weapon_loadout_file;
			ImGui::PushItemWidth(250);
			if (ImGui::BeginListBox("Saved Loadouts", ImVec2(200, 200)))
			{
				for (std::string filename : persist_weapons::list_weapon_loadouts())
				{
					if (components::selectable(filename, filename == selected_loadout))
					{
						selected_loadout = filename;
					}
				}
				ImGui::EndListBox();
			}
			ImGui::SameLine();
			ImGui::BeginGroup();
			static std::string input_file_name;
			components::input_text_with_hint("Weapon Loadout Filename", "Loadout Name", input_file_name);
			components::button("Save Loadout", [] {
				persist_weapons::save_weapons(input_file_name);
				input_file_name.clear();
			});
			ImGui::SameLine();
			components::button("Load Loadout", [] {
				persist_weapons::give_player_loadout(selected_loadout);
			});
			ImGui::SameLine();
			components::button("Set Loadout", [] {
				persist_weapons::set_weapon_loadout(selected_loadout);
			});
			ImGui::Text("%s: %s:", "Current Loadout", g.persist_weapons.weapon_loadout_file.c_str());
			ImGui::EndGroup();
			ImGui::PopItemWidth();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Weapon Hotkeys"))
		{
			ImGui::PushID(2);
			ImGui::Checkbox("Enabled", &g.weapons.enable_weapon_hotkeys);
			ImGui::PopID();
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("%s", "This will select the next weapon in the hotkey list.\nThe first weapon in the list is the first weapon it will select, then the second is the one it will select after and so on.\nAfter the end of the list, it will wrap back to the first weapon.");
			}

			static int selected_key = 0;
			const char* const keys[]{"1", "2", "3", "4", "5", "6"};

			ImGui::PushItemWidth(250);
			ImGui::Combo("Key", &selected_key, keys, IM_ARRAYSIZE(keys));
			ImGui::PopItemWidth();

			if (!g.weapons.weapon_hotkeys[selected_key].empty())
			{
				int counter{};
				for (auto& weapon_hash : g.weapons.weapon_hotkeys[selected_key])
				{
					ImGui::PushID(counter);
					weapon_item weapon = g_gta_data_service->weapon_by_hash(weapon_hash);
					ImGui::PushItemWidth(300);
					if (ImGui::BeginCombo("Weapons", weapon.m_display_name.c_str()))
					{
						std::map<std::string, weapon_item> sorted_map;
						for (const auto& [_, weapon_iter] : g_gta_data_service->weapons())
						{
							sorted_map.emplace(weapon_iter.m_display_name, weapon_iter);
						}
						for (const auto& [_, weapon_iter] : g_gta_data_service->weapons())
						{
							if (weapon_iter.m_display_name == "NULL")
							{
								continue;
							}
							bool is_selected = weapon_iter.m_hash == weapon_hash;
							if (ImGui::Selectable(weapon_iter.m_display_name.c_str(), is_selected, ImGuiSelectableFlags_None))
							{
								weapon_hash = weapon_iter.m_hash;
							}
							if (is_selected)
							{
								ImGui::SetItemDefaultFocus();
							}
						}
						ImGui::EndCombo();
					}
					ImGui::SameLine();
					components::button("Set To Current Weapon", [&weapon_hash] {
						WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, 0);
						if (weapon_hash == 0)
						{
							WEAPON::GET_CURRENT_PED_VEHICLE_WEAPON(self::ped, &weapon_hash);
						}
					});
					ImGui::SameLine();
					if (ImGui::Button("Remove Weapon"))
					{
						g.weapons.weapon_hotkeys[selected_key].erase(g.weapons.weapon_hotkeys[selected_key].begin() + counter);
					}
					ImGui::PopID();
					ImGui::PopItemWidth();
					counter++;
				}
			}

			if (ImGui::Button("Add Weapon"))
			{
				g.weapons.weapon_hotkeys[selected_key].push_back("WEAPON_UNARMED"_J);
			}
			ImGui::TreePop();
		}
	}
}
