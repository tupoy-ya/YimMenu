#pragma once

#include "common.hpp"

#include "services/players/player_service.hpp"

namespace big
{
	class looped {
	public:
		static void hud_transition_state();

		static void tunables_disable_phone();
		static void phone_anim(); 
		static void tunables_no_idle_kick();
		static void always_controll();
		static void disable_help_text();

		static void player_good_options();
		static void player_toxic_options();
		static void player_spectate();
		static void player_remote_control_vehicle();

		static void self_godmode();
		static void self_police();

		static void ragdoll_mp();
		static void self_hud();
		static void self_dance_mode();

		static void script_starter();
		static void session_local_time();
		static void seamless_join();
		
		static void world_population_control();
		static void session_pop_multiplier_areas();
		static void session_force_thunder();
		static void session_randomize_ceo_colors();
		static void session_auto_kick_host();

		static void system_self_globals();
		static void system_update_pointers();
		static void system_desync_kick_protection();
		static void system_spoofing();
		static void system_mission_creator();
		static void system_auto_tp();

		static void vehicle_auto_drive();
		static void vehicle_boost_behavior();
		static void vehicle_fly();
		static void vehicle_god_mode();
		static void vehicle_ls_customs();
		static void vehicle_rainbow_paint();
		static void vehicle_speedo_meter();
		static void vehicle_turn_signals();
		static void vehicle_flares();
		static void vehicle_chaff();

		static void weapons_ammo_special_type();
		static void weapons_cage_gun();
		static void custom_gun_disable_control_action();
		static void weapons_delete_gun();
		static void weapons_gravity_gun();
		static void weapons_increased_damage();
		static void weapons_repair_gun();
		static void weapons_steal_vehicle_gun();
		static void weapons_vehicle_gun();

		static void drive_train();
		static void derail_train();
	};
}
