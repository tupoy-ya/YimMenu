#pragma once
#include "gta/joaat.hpp"
#include "natives.hpp"
#include "script.hpp"
#include "math.hpp"
#include "util/notify.hpp"
#include "pointers.hpp"

namespace big::entity
{
	inline void cage_ped(Ped ped)
	{
		Hash hash = RAGE_JOAAT("prop_gold_cont_01");

		Vector3 location = ENTITY::GET_ENTITY_COORDS(ped, true);
		OBJECT::CREATE_OBJECT(hash, location.x, location.y, location.z - 1.f, true, false, false);
	}

	inline int spawn_ped(const char* model, Vector3 location)
	{
		{
			Hash hash = rage::joaat(model);

			if (hash)
			{
				for (uint8_t i = 0; !STREAMING::HAS_MODEL_LOADED(hash) && i < 100; i++)
				{
					STREAMING::REQUEST_MODEL(hash);

					script::get_current()->yield();
				}
				if (!STREAMING::HAS_MODEL_LOADED(hash))
				{
					notify::above_map("~r~Failed to spawn model, did you give an incorrect model?");

					return -1;
				}

				int pedtype = PED::GET_PED_TYPE(hash);
				*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x9090;
				Ped ped = PED::CREATE_PED(pedtype, hash, location.x, location.y, location.z, 1.f, true, true);
				*(unsigned short*)g_pointers->m_model_spawn_bypass = 0x0574;

				script::get_current()->yield();

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

				return ped;
			}
			return -1;
		}
	}

	inline int spawn_prop(const char* model, Vector3 location)
	{
		{
			Hash hash = rage::joaat(model);

			if (hash)
			{
				for (uint8_t i = 0; !STREAMING::HAS_MODEL_LOADED(hash) && i < 100; i++)
				{
					STREAMING::REQUEST_MODEL(hash);

					script::get_current()->yield();
				}
				if (!STREAMING::HAS_MODEL_LOADED(hash))
				{
					notify::above_map("~r~Failed to spawn model, did you give an incorrect model?");

					return -1;
				}

				Object object = OBJECT::CREATE_OBJECT(hash, location.x, location.y, location.z, true, false, false);

				script::get_current()->yield();

				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(hash);

				return object;
			}
			return -1;
		}
	}

	inline void delete_entity(Entity ent)
	{
		ENTITY::DETACH_ENTITY(ent, 1, 1);
		ENTITY::SET_ENTITY_VISIBLE(ent, false, false);
		NETWORK::NETWORK_SET_ENTITY_INVISIBLE_TO_NETWORK_(ent, true);
		ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, 0, 0, 0, 0, 0, 0);
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(ent, 1, 1);
		ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&ent);
		ENTITY::DELETE_ENTITY(&ent);
	}

	inline bool raycast(Entity* ent)
	{
		BOOL hit;
		Vector3 endCoords;
		Vector3 surfaceNormal;

		Vector3 camCoords = CAM::GET_GAMEPLAY_CAM_COORD();
		Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
		Vector3 dir = math::rotation_to_direction(rot);
		Vector3 farCoords;

		farCoords.x = camCoords.x + dir.x * 1000;
		farCoords.y = camCoords.y + dir.y * 1000;
		farCoords.z = camCoords.z + dir.z * 1000;

		int ray = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(camCoords.x, camCoords.y, camCoords.z, farCoords.x, farCoords.y, farCoords.z, -1, 0, 7);
		SHAPETEST::GET_SHAPE_TEST_RESULT(ray, &hit, &endCoords, &surfaceNormal, ent);

		return (bool)hit;
	}

	inline bool take_control_of(Entity ent)
	{
		if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(ent)) return true;
		for (uint8_t i = 0; !NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(ent) && i < 10; i++)
		{
			NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(ent);

			script::get_current()->yield(5ms);
		}
		if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(ent)) return false;

		int netHandle = NETWORK::NETWORK_GET_NETWORK_ID_FROM_ENTITY(ent);
		NETWORK::SET_NETWORK_ID_CAN_MIGRATE(netHandle, true);

		return true;
	}
}