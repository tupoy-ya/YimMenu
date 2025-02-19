#pragma once
#include "gta/enums.hpp"
#include "natives.hpp"
#include "script_global.hpp"
#include "system.hpp"

namespace big::toxic
{
	inline void blame_explode_coord(Player to_blame, Vector3 pos, int explosion_type, float damage, bool is_audible, bool is_invisible, float camera_shake)
	{
		system::patch_blame(true);
		FIRE::ADD_OWNED_EXPLOSION(
			PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(to_blame),
			pos.x, pos.y, pos.z,
			(int)explosion_type,
			damage,
			is_audible,
			is_invisible,
			camera_shake
		);
		system::patch_blame(false);
	}

	inline void blame_explode_player(Player to_blame, Player target, int explosion_type, float damage, bool is_audible, bool is_invisible, float camera_shake)
	{
		Vector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(target), true);
		blame_explode_coord(to_blame, coords, explosion_type, damage, is_audible, is_invisible, camera_shake);
	}

	// param 0 == should send
	// param 1 == player target
	// param 2 == amount
	// param 3 == unk? => always 1
	// param 4 == if some kind of bit must be set 
	inline void bounty_player(Player target, Player origin, int amount)
	{
		const size_t arg_count = 22;
		int args[arg_count] = {
			(int)eRemoteEvent::Bounty,
			0, // doesn't matter of we set this to something else, the TRIGGER_SCRIPT_EVENT routine will set it to our player id anyways
			target,
			0, // set by player or NPC?
			10000,
			0, 1, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0,
			*script_global(1921036).at(9).as<int*>(),
			*script_global(1921036).at(10).as<int*>()
		};
	}

	inline void ceo_kick(Player target)
	{
		const size_t arg_count = 4;
		int ceo_kick[4] = { (int)eRemoteEvent::CeoBan , PLAYER::PLAYER_ID(), 1, 5 };

		g_pointers->m_trigger_script_event(1, ceo_kick, arg_count, 1 << target);
	}

	inline void ForceMission(Player target)
	{
		int args[2] = { (int)eRemoteEvent::ForceMission, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(target) };

		g_pointers->m_trigger_script_event(true, args, 2, 1 << target);
	}

	inline void kick(Player target)
	{
		int args[4] = { 0, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(target), 0, 0 };

		for (int64_t kick_hash : kick_hashes)
		{
			args[0] = kick_hash;

			g_pointers->m_trigger_script_event(true, args, 4, 1 << g_player_service->m_selected_player->id());
		}
	}

	//-1813981910
	inline void crash_event(Player target)
	{
		int args[4] = { -2043109205, target, 0, 0 };

			g_pointers->m_trigger_script_event(true, args, 4, 1 << g_player_service->m_selected_player->id());
	}

	inline void SendToIsland(Player target)
	{
		const size_t arg_count = 4;
		int args[4] = { (int)eRemoteEvent::SendToIsland , PLAYER::PLAYER_ID(), 1, 5 };

		g_pointers->m_trigger_script_event(1, args, arg_count, 1 << target);
	}

	inline void SoundSpam(Player target)
	{
		const size_t arg_count = 4;
		int args[4] = { (int)eRemoteEvent::SoundSpam , PLAYER::PLAYER_ID(), 1, 5 };

		g_pointers->m_trigger_script_event(1, args, arg_count, 1 << target);
	}
}