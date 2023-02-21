#include "natives.hpp"
#include "pointers.hpp"
#include "thread_pool.hpp"
#include "util/system.hpp"
#include "network/Network.hpp"
#include "gta_util.hpp"
#include "views/view.hpp"

namespace big
{
	void view::debug_misc() 
	{
		components::button("Load MP Map", [] { DLC::ON_ENTER_MP(); });
		ImGui::SameLine();
		components::button("Load SP Map", [] { DLC::ON_ENTER_SP(); });

		if (components::button("Dump entrypoints"))
		{
			system::dump_entry_points();
		}
		ImGui::SameLine();
		components::button("Network Bail", []
		{
			NETWORK::NETWORK_BAIL(16, 0, 0);
		});

		components::button("Hide news", []
		{
			SOCIALCLUB::SC_TRANSITION_NEWS_END();
		});
		ImGui::SameLine();
		components::button("Network Session Host", []
		{
			NETWORK::NETWORK_SESSION_HOST(1, 32, false);
		});
		components::button("Refresh Interior", []
		{
			Interior interior = INTERIOR::GET_INTERIOR_AT_COORDS(self::pos.x, self::pos.y, self::pos.z);
			INTERIOR::REFRESH_INTERIOR(interior);
		});
		ImGui::SameLine(); components::help_marker("You Will Have To Refresh Again When Exiting Interior.\n SPAMMING WILL CRASH GAME");

		components::button("Network Shutdown And Launch Single Player Game", []
		{
			NETWORK::SHUTDOWN_AND_LAUNCH_SINGLE_PLAYER_GAME();
		});

		components::button("Network Shutdown And Load Most Recent Save", []\
		{
			NETWORK::SHUTDOWN_AND_LOAD_MOST_RECENT_SAVE();
		});

		if (components::button("MOV QWORD"))
		{
			*static_cast<uint64_t*>(nullptr) = 0;
			uint64_t i = *static_cast<uint64_t*>(nullptr);
		}
		ImGui::SameLine();
		if (components::button("MOV 0xdead"))
		{
			*((unsigned int*)0) = 0xDEAD;
		}

		if (g_local_player && g_local_player->m_player_info)
		{
			ImGui::InputScalar("Rockstar ID", ImGuiDataType_S64, &g_local_player->m_player_info->m_net_player_data.m_gamer_handle.m_rockstar_id, nullptr, nullptr, nullptr, ImGuiInputTextFlags_ReadOnly);
		}

		ImGui::Checkbox("Log Native Script Hooks", &g.debug.logs.script_hook_logs);
		ImGui::Checkbox("Enable Stupid Script Native Logs", &g.debug.logs.stupid_script_native_logs);

		ImGui::Checkbox("Log Packets", &g.debug.logs.packet_logs);
		ImGui::Checkbox("Log Net Events", &g.debug.logs.net_event_logs);

		ImGui::Checkbox("Log Remote Sounds", &g.debug.logs.remote_sound_logs);
		
		ImGui::Checkbox("Log Metrics", &g.debug.logs.metric_logs);

		if (ImGui::TreeNode("Script Event Logging"))
		{
			ImGui::Checkbox("Enable Script Event Logging", &g.debug.logs.script_event.logs);
			ImGui::Separator();

			ImGui::Checkbox("Filter by Player", &g.debug.logs.script_event.filter_player);

			if (g.debug.logs.script_event.filter_player)
			{
				ImGui::ListBoxHeader("##filter_player");
				for (const auto& [_, player] : g_player_service->players())
				{
					if (components::selectable(player->get_name(), g.debug.logs.script_event.player_id == player->id()))
					{
						g.debug.logs.script_event.player_id = player->id();
					}
				}
				ImGui::EndListBox();
			}
			ImGui::Checkbox("Block All", &g.debug.logs.script_event.block_all);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Addresses"))
		{
			uint64_t local_cped = (uint64_t)g_local_player;
			ImGui::InputScalar("Local CPed", ImGuiDataType_U64, &local_cped, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal);

			if (g_local_player)
			{
				uint64_t local_playerinfo = (uint64_t)g_local_player->m_player_info;
				ImGui::InputScalar("Local CPlayerInfo", ImGuiDataType_U64, &local_playerinfo, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal);

				uint64_t local_vehicle = (uint64_t)g_local_player->m_vehicle;
				ImGui::InputScalar("Local CAutomobile", ImGuiDataType_U64, &local_vehicle, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal);
			}

			if (auto mgr = *g_pointers->m_network_player_mgr)
			{
				uint64_t local_netplayer = (uint64_t)mgr->m_local_net_player;
				ImGui::InputScalar("Local CNetGamePlayer", ImGuiDataType_U64, &local_netplayer, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal);

				if (mgr->m_local_net_player)
				{
					uint64_t local_netplayer = (uint64_t)mgr->m_local_net_player->get_net_data();
					ImGui::InputScalar("Local netPlayerData", ImGuiDataType_U64, &local_netplayer, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal);
				}
			}

			if (auto network = *g_pointers->m_network)
			{
				uint64_t nw = (uint64_t)network;
				ImGui::InputScalar("Network", ImGuiDataType_U64, &nw, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal);
			}

			if (auto omgr = *g_pointers->m_network_object_mgr)
			{
				uint64_t nw = (uint64_t)omgr;
				ImGui::InputScalar("Network Object Mgr", ImGuiDataType_U64, &nw, NULL, NULL, "%p", ImGuiInputTextFlags_CharsHexadecimal);
			}

			ImGui::TreePop();
		}

		components::command_button<"fastquit">();
	}
}