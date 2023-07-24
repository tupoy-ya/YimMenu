#include "backend/command.hpp"
#include "natives.hpp"
#include "util/ped.hpp"

namespace big
{
	class clean_player : command
	{
		using command::command;

		virtual void execute(const std::vector<uint64_t>&, const std::shared_ptr<command_context> ctx)
		{
			ped::clean_ped(self::ped);
		}
	};

	clean_player g_clean_player("clean", "Clean Player", "Cleans the player of wetness and decals", 0);
}