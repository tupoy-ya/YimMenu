#include "hooking.hpp"
#include "pointers.hpp"
#include "services/players/player_service.hpp"

#include <network/CNetGamePlayer.hpp>
#include <player/CNonPhysicalPlayerData.hpp>

namespace big
{
	void hooks::send_non_physical_player_data(CNetGamePlayer* player, __int64 message, int flags, void* a4, CNetGamePlayer* a5)
	{
		auto plyr         = g_player_service->get_by_id(player->m_player_id);
		auto data         = *(CNonPhysicalPlayerData**)(message + 0x10);
		int old_bubble_id = data->m_bubble_id;

		if (plyr && plyr->block_join && *g_pointers->m_gta.m_is_session_started)
		{
			data->m_bubble_id = 10;
			g_notification_service->push("Block Join", std::vformat("Trying to prevent {} from joining...", std::make_format_args(plyr->get_name())));
		}

		g_hooking->get_original<hooks::send_non_physical_player_data>()(player, message, flags, a4, a5);

		data->m_bubble_id = old_bubble_id;
	}
}