/**
 * @file packet.cpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#include "packet.hpp"

#include "common.hpp"
#include "gta_util.hpp"
#include "services/players/player_service.hpp"

#include <network/Network.hpp>
#include <network/netConnection.hpp>

namespace big
{
	packet::packet() :
	    m_buffer(m_data, sizeof(m_data))
	{
	}

	void packet::send(uint32_t msg_id)
	{
		g_pointers->m_gta
		    .m_queue_packet(gta_util::get_network()->m_game_session_ptr->m_net_connection_mgr, msg_id, m_data, (m_buffer.m_curBit + 7) >> 3, 1, nullptr);
	}

	void packet::send(player_ptr player, int connection_id)
	{
		send(player->get_session_player()->m_player_data.m_peer_id_2, connection_id);
	}

	void packet::send(int peer_id, int connection_id)
	{
		auto mgr  = gta_util::get_network()->m_game_session_ptr->m_net_connection_mgr;
		auto peer = g_pointers->m_gta.m_get_connection_peer(mgr, peer_id);
		g_pointers->m_gta.m_send_packet(mgr, &peer->m_peer_address, connection_id, m_data, (m_buffer.m_curBit + 7) >> 3, 0x1000000);
	}
}