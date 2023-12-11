/**
 * @file send_chat_message.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "backend/command.hpp"
#include "backend/context/chat_command_context.hpp"
#include "gta/net_game_event.hpp"
#include "gta_util.hpp"
#include "hooking.hpp"
#include "packet.hpp"
#include "services/players/player_service.hpp"
#include "util/spam.hpp"

#if defined(ENABLE_LUA)
#include "lua/lua_manager.hpp"
#endif // ENABLE_LUA

namespace big
{
	inline void gamer_handle_serialize(rage::rlGamerHandle& hnd, rage::datBitBuffer& buf)
	{
		constexpr int PC_PLATFORM = 3;
		buf.Write<uint8_t>(PC_PLATFORM, 8);
		buf.WriteInt64(*(int64_t*)&hnd.m_rockstar_id, 64);
		buf.Write<uint8_t>(hnd.unk_0009, 8);
	}

	bool hooks::send_chat_message(void* team_mgr, rage::rlGamerInfo* local_gamer_info, char* message, bool is_team)
	{
		if (g.session.chat_commands && message[0] == g.session.chat_command_prefix)
			command::process(std::string(message + 1), std::make_shared<chat_command_context>(g_player_service->get_self()));
#if defined(ENABLE_LUA)
		else
			g_lua_manager->trigger_event<menu_event::ChatMessageReceived>(self::id, message);
#endif // ENABLE_LUA

		packet msg{};
		msg.write_message(rage::eNetMessage::MsgTextMessage);
		msg.m_buffer.WriteString(message ? message : "", 256);
		gamer_handle_serialize(g_player_service->get_self()->get_net_data()->m_gamer_handle, msg.m_buffer);
		msg.write<bool>(is_team, 1);

		if (g.session.log_chat_messages)
			spam::log_chat(message, g_player_service->get_self(), SpamReason::NOT_A_SPAMMER, is_team);

		if (*g_pointers->m_gta.m_is_session_started)
			for (auto& player : g_player_service->players())
				if (player.second && player.second->is_valid())
					msg.send(player.second->get_net_game_player()->m_msg_id);

		return true;
	}
}
