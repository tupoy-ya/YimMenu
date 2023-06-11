/**
 * @file view_weapons.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "core/data/bullet_impact_types.hpp"
#include "core/data/custom_weapons.hpp"
#include "core/data/special_ammo_types.hpp"
#include "fiber_pool.hpp"
#include "gta/joaat.hpp"
#include "imgui.h"
#include "natives.hpp"
#include "pointers.hpp"
#include "services/gta_data/gta_data_service.hpp"
#include "views/view.hpp"

namespace big
{
	void view::weapons()
	{
		ImGui::Text("Ammo");

		ImGui::BeginGroup();
		components::command_checkbox<"infammo">();
		components::command_checkbox<"infrange">();
		ImGui::Checkbox("Increased C4 Limit (Max = 50)", &g.weapons.increased_c4_limit);
		ImGui::Checkbox("Allow Weapons In Interiors", &g.weapons.interior_weapon);
		components::command_checkbox<"rapidfire">();

		ImGui::EndGroup();
		ImGui::SameLine();
		ImGui::BeginGroup();

		components::command_checkbox<"infammoloop">();
		components::command_checkbox<"infclip">();
		ImGui::Checkbox("Increased Flare Limit (Max = 50)", &g.weapons.increased_flare_limit);
		components::command_checkbox<"vehallweapons">();

		ImGui::EndGroup();


		ImGui::SeparatorText("Special Ammo");


		ImGui::Checkbox("Enable Special Ammo", &g.weapons.ammo_special.toggle);

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

		ImGui::SeparatorText("Misc");

		components::command_checkbox<"norecoil">();
		ImGui::SameLine();
		components::command_checkbox<"nospread">();

		components::button("Get All Weapons", [] {
			for (const auto& [_, weapon] : g_gta_data_service->weapons())
			{
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(self::ped, weapon.m_hash, 9999, false);
			}

			constexpr auto parachute_hash = RAGE_JOAAT("GADGET_PARACHUTE");
			WEAPON::GIVE_DELAYED_WEAPON_TO_PED(self::ped, parachute_hash, 0, true);
		});
		ImGui::SameLine();
		components::button("Remove Current Weapon", [] {
			Hash weaponHash;
			WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weaponHash, 1);
			if (weaponHash != RAGE_JOAAT("WEAPON_UNARMED"))
			{
				WEAPON::REMOVE_WEAPON_FROM_PED(self::ped, weaponHash);
			}
		});

		components::command_checkbox<"incrdamage">();
		ImGui::InputFloat("Damage", &g.weapons.increased_damage, .1, 10, "%.1f");

		ImGui::SeparatorText("Custom Weapons");

		ImGui::Checkbox("Custom Gun only fires when weapon is out", &g.self.custom_weapon_stop);
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
			{
				g_fiber_pool->queue_job([] {
					PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
				});
			}

			break;
		}
	}
}
