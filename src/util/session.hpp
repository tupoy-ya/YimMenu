/**
 * @file session.hpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "core/data/session_types.hpp"
#include "fiber_pool.hpp"
#include "gta/joaat.hpp"
#include "gta_util.hpp"
#include "natives.hpp"
#include "packet.hpp"
#include "pointers.hpp"
#include "rage/rlSessionByGamerTaskResult.hpp"
#include "script.hpp"
#include "script_global.hpp"
#include "services/player_database/player_database_service.hpp"
#include "services/players/player_service.hpp"
#include "thread_pool.hpp"
#include "util/globals.hpp"
#include "util/misc.hpp"

#include <network/Network.hpp>
#include <network/snConnectToPeerTask.hpp>
#include <rage/rlQueryPresenceAttributesContext.hpp>
#include <rage/rlScHandle.hpp>

namespace big::session
{
	static void gamer_handle_serialize(rage::rlGamerHandle& hnd, rage::datBitBuffer& buf)
	{
		buf.Write<uint8_t>(*reinterpret_cast<uint8_t*>(&hnd.m_platform), 8);
		if (*reinterpret_cast<uint8_t*>(&hnd.m_platform) == 3)
		{
			buf.WriteInt64(*(int64_t*)&hnd.m_rockstar_id, 64);
			buf.Write<uint8_t>(*reinterpret_cast<uint8_t*>(reinterpret_cast<__int64>(&hnd) + 9), 8);
		}
	}

	/**
	 * @brief Join session by type.
	 * 
	 * @param session Session type. 
	 */
	inline void join_type(eSessionType session)
	{
		*script_global(2695915).as<int*>() = (session == eSessionType::SC_TV ? 1 : 0); // If SC TV Then Enable Spectator Mode

		if (session == eSessionType::LEAVE_ONLINE)
			*script_global(1574589).at(2).as<int*>() = -1;
		else
			*script_global(1575017).as<int*>() = (int)session;

		*script_global(1574589).as<int*>() = 1;
		script::get_current()->yield(200ms);
		*script_global(1574589).as<int*>() = 0;
	}

	inline void set_fm_event_index(int index)
	{
		int idx = index / 32;
		int bit = index % 32;
		misc::set_bit(scr_globals::gsbd_fm_events.at(11).at(354).at(idx, 1).as<int*>(), bit);
		misc::set_bit(scr_globals::gsbd_fm_events.at(11).at(347).at(idx, 1).as<int*>(), bit);
		misc::set_bit(scr_globals::gpbd_fm_3.at(self::id, scr_globals::size::gpbd_fm_3).at(10).at(205).at(idx, 1).as<int*>(), bit);
	}

	inline void clear_fm_event_index(int index)
	{
		int idx = index / 32;
		int bit = index % 32;
		misc::clear_bit(scr_globals::gsbd_fm_events.at(11).at(354).at(idx, 1).as<int*>(), bit);
		misc::clear_bit(scr_globals::gsbd_fm_events.at(11).at(347).at(idx, 1).as<int*>(), bit);
		misc::clear_bit(scr_globals::gpbd_fm_3.at(self::id, scr_globals::size::gpbd_fm_3).at(10).at(205).at(idx, 1).as<int*>(), bit);
	}

	/**
	 * @brief Joins session by is't rage::rlSessionInfo.
	 * 
	 * @param info Session info to join.
	 */
	inline void join_session(const rage::rlSessionInfo& info)
	{
		g.session.join_queued = true;
		g.session.info        = info;
		session::join_type({eSessionType::NEW_PUBLIC});
		if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(RAGE_JOAAT("maintransition")) == 0)
		{
			g.session.join_queued = false;
			g_notification_service->push_error("RID Joiner", "Unable to launch maintransition");
		}
		return;
	}

	/**
	 * @brief Joines player by Rockstar ID.
	 * @param rid RID to join.
	*/
	inline void join_by_rockstar_id(uint64_t rid)
	{
		if (SCRIPT::GET_NUMBER_OF_THREADS_RUNNING_THE_SCRIPT_WITH_THIS_HASH(RAGE_JOAAT("maintransition")) != 0 || STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
		{
			g_notification_service->push_error("RID Joiner", "Player switch in progress, wait a bit.");
			return;
		}

		rage::rlGamerHandle player_handle(rid);
		rage::rlSessionByGamerTaskResult result;
		bool success = false;
		rage::rlTaskStatus state{};

		if (g_pointers->m_gta.m_start_get_session_by_gamer_handle(0, &player_handle, 1, &result, 1, &success, &state))
		{
			while (state.status == 1)
				script::get_current()->yield();

			if (state.status == 3 && success)
			{
				join_session(result.m_session_info);
				return;
			}
		}

		g_notification_service->push_error("RID Joiner", "Target player is offline?");
	}

	/**
	 * @brief Adds player to the database and assigns infraction.
	 * 
	 * @param player Player to add infraction to.
	 * @param infraction Infraction to add.
	 */
	inline void add_infraction(player_ptr player, Infraction infraction)
	{
		LOG(INFO) << std::format("Anti-Cheat: {} - {}", player->get_name(), infraction_desc[infraction]);
		auto plyr = g_player_database_service->get_or_create_player(player);
		if (!plyr->infractions.contains((int)infraction))
		{
			plyr->is_modder   = true;
			player->is_modder = true;
			plyr->infractions.insert((int)infraction);
			g_player_database_service->save();
		}
	}

	inline void give_collectible(Player target, eCollectibleType col, int index = 0, bool uncomplete = false)
	{
		const size_t arg_count  = 7;
		int64_t args[arg_count] = {
		    (int64_t)eRemoteEvent::GiveCollectible,
		    (int64_t)self::id,
		    (int64_t)col,   // iParam0
		    (int64_t)index, // iParam1
		    !uncomplete,    // bParam2
		    true,
		    0 // bParam3
		};

		g_pointers->m_gta.m_trigger_script_event(1, args, arg_count, 1 << target);
	}
}
