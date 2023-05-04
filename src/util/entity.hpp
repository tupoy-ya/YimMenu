/**
 * @file entity.hpp
 * @brief Basic entity related functions.
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "gta/joaat.hpp"
#include "gta/replay.hpp"
#include "gta_util.hpp"
#include "math.hpp"
#include "natives.hpp"
#include "pools.hpp"
#include "script.hpp"

namespace big::entity
{
	/**
	 * @brief Checks if you have control over net_object
	 * 
	 * @param net_object Entity to check control of.
	 * @return True if you have control over entity.
	 */
	inline bool network_has_control_of_entity(rage::netObject* net_object)
	{
		return !net_object || !net_object->m_next_owner_id && (net_object->m_control_id == -1);
	}

	/**
	 * @brief Attemts to take control of given entity
	 * 
	 * @param ent Entity to take control of.
	 * @param timeout When to give up trying to take control. In for loop iterations. 
	 * @return True if the control has been taken.
	 */
	inline bool take_control_of(Entity ent, int timeout = 300)
	{
		if (!*g_pointers->m_gta.m_is_session_started)
			return true;

		auto hnd = g_pointers->m_gta.m_handle_to_ptr(ent);

		if (!hnd || !hnd->m_net_object || !*g_pointers->m_gta.m_is_session_started)
			return false;

		if (network_has_control_of_entity(hnd->m_net_object))
			return true;

		for (int i = 0; i < timeout; i++)
		{
			g_pointers->m_gta.m_request_control(hnd->m_net_object);

			if (network_has_control_of_entity(hnd->m_net_object))
				return true;

			if (timeout != 0)
				script::get_current()->yield();
		}

		return false;
	}

	/**
	 * @brief Deletes given entity.
	 * Does the following steps to remove an entity 
	 * 1. Removes all attachments of this entity.
	 * 2. Makes the entity invisible.
	 * 3. Teleports the entity to X: 15000 Y: 15000 Z: 10000.
	 * 4. Marks the entity as mission entity.
	 * 5. Deletes the entity.
	 * 
	 * @note Attempts to take control of entity on it's own.
	 * If it cannot take control it will show a "Delete entity failed." warning.
	 * 
	 * @param ent Entity to delete
	 */
	inline void delete_entity(Entity ent)
	{
		if (take_control_of(ent, 1000))
		{
			ENTITY::DETACH_ENTITY(ent, 1, 1);
			ENTITY::SET_ENTITY_VISIBLE(ent, false, false);
			NETWORK::NETWORK_SET_ENTITY_ONLY_EXISTS_FOR_PARTICIPANTS(ent, true);
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, 15'000, 15'000, 10'000, 0, 0, 0);
			ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ent, 1, 1);
			ENTITY::DELETE_ENTITY(&ent);
		}
		else
		{
			g_notification_service->push_warning("Delete Entity", "Delete entity failed.");
		}
	}

	/**
	 * @brief Raycasts for entitys from the current camera forward.
	 * 
	 * @param ent Entity that we have hit.
	 * @return True if hit something other then the sky.
	 */
	inline bool raycast(Entity* ent)
	{
		BOOL hit;
		Vector3 endCoords;
		Vector3 surfaceNormal;

		Vector3 camCoords = CAM::GET_GAMEPLAY_CAM_COORD();
		Vector3 rot       = CAM::GET_GAMEPLAY_CAM_ROT(2);
		Vector3 dir       = math::rotation_to_direction(rot);
		Vector3 farCoords;

		farCoords.x = camCoords.x + dir.x * 1000;
		farCoords.y = camCoords.y + dir.y * 1000;
		farCoords.z = camCoords.z + dir.z * 1000;

		int ray = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(camCoords.x,
		    camCoords.y,
		    camCoords.z,
		    farCoords.x,
		    farCoords.y,
		    farCoords.z,
		    -1,
		    0,
		    7);
		SHAPETEST::GET_SHAPE_TEST_RESULT(ray, &hit, &endCoords, &surfaceNormal, ent);

		return (bool)hit;
	}

	/**
	 * @brief Gets all entitys from the replay interface.
	 * 
	 * @note Does not include local player or local vehicle.
	 * 
	 * @param vehicles Include vehicles.
	 * @param peds Include peds.
	 * @return std::vector<Entity> of all entitys in the replay interface.
	 */
	inline std::vector<Entity> get_entities(bool vehicles, bool peds)
	{
		std::vector<Entity> target_entities;
		target_entities.clear();

		if (vehicles)
		{
			for (auto vehicle : pools::get_all_vehicles())
			{
				if (vehicle == gta_util::get_local_vehicle())
					continue;

				target_entities.push_back(g_pointers->m_gta.m_ptr_to_handle(vehicle));
			}
		}

		if (peds)
		{
			for (auto ped : pools::get_all_peds())
			{
				// make sure to not include ourselves
				if (ped == gta_util::get_local_ped())
					continue;

				target_entities.push_back(g_pointers->m_gta.m_ptr_to_handle(ped));
			}
		}
		return target_entities;
	}

	inline bool load_ground_at_3dcoord(Vector3& location)
	{
		float groundZ;
		const uint8_t attempts = 10;

		for (uint8_t i = 0; i < attempts; i++)
		{
			// Only request a collision after the first try failed because the location might already be loaded on first attempt.
			for (uint16_t z = 0; i && z < 1000; z += 100)
			{
				STREAMING::REQUEST_COLLISION_AT_COORD(location.x, location.y, (float)z);

				script::get_current()->yield();
			}

			if (MISC::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, 1000.f, &groundZ, false, false))
			{
				location.z = groundZ + 1.f;

				return true;
			}

			script::get_current()->yield();
		}

		location.z = 1000.f;

		return false;
	}
}