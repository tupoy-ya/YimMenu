#include "backend/looped_command.hpp"
#include "natives.hpp"

namespace big
{
	class remove_speed_limit : looped_command
	{
		using looped_command::looped_command;

		virtual void on_tick() override
		{
			if (self::veh != 0)
				ENTITY::SET_ENTITY_MAX_SPEED(self::veh, 999999999999.f);
			else
				ENTITY::SET_ENTITY_MAX_SPEED(self::ped, 999999999999.f);
		}

		virtual void on_disable() override
		{
			if (self::veh != 0)
				ENTITY::SET_ENTITY_MAX_SPEED(self::veh, 1000); // TODO: Use real default value.
			else
				ENTITY::SET_ENTITY_MAX_SPEED(self::ped, 1000);
		}
	};

	remove_speed_limit
	    g_remove_speed_limit("remove_speed_limit", "Remove Speed Limit", "Lets you go faster then ~540 km/h", g.tunables.remove_speed_limit);
}
