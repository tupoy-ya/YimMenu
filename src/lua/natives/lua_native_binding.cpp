#if defined (ENABLE_LUA)
#include "lua_native_binding.hpp"

namespace lua::native
{
	void init_native_binding(sol::state& L)
	{
		init_native_binding_SYSTEM(L);
		init_native_binding_APP(L);
		init_native_binding_AUDIO(L);
		init_native_binding_BRAIN(L);
		init_native_binding_CAM(L);
		init_native_binding_CLOCK(L);
		init_native_binding_CUTSCENE(L);
		init_native_binding_DATAFILE(L);
		init_native_binding_DECORATOR(L);
		init_native_binding_DLC(L);
		init_native_binding_ENTITY(L);
		init_native_binding_EVENT(L);
		init_native_binding_FILES(L);
		init_native_binding_FIRE(L);
		init_native_binding_GRAPHICS(L);
		init_native_binding_HUD(L);
		init_native_binding_INTERIOR(L);
		init_native_binding_ITEMSET(L);
		init_native_binding_LOADINGSCREEN(L);
		init_native_binding_LOCALIZATION(L);
		init_native_binding_MISC(L);
		init_native_binding_MOBILE(L);
		init_native_binding_MONEY(L);
		init_native_binding_NETSHOPPING(L);
		init_native_binding_NETWORK(L);
		init_native_binding_OBJECT(L);
		init_native_binding_PAD(L);
		init_native_binding_PATHFIND(L);
		init_native_binding_PED(L);
		init_native_binding_PHYSICS(L);
		init_native_binding_PLAYER(L);
		init_native_binding_RECORDING(L);
		init_native_binding_REPLAY(L);
		init_native_binding_SAVEMIGRATION(L);
		init_native_binding_SCRIPT(L);
		init_native_binding_SECURITY(L);
		init_native_binding_SHAPETEST(L);
		init_native_binding_SOCIALCLUB(L);
		init_native_binding_STATS(L);
		init_native_binding_STREAMING(L);
		init_native_binding_TASK(L);
		init_native_binding_VEHICLE(L);
		init_native_binding_WATER(L);
		init_native_binding_WEAPON(L);
		init_native_binding_ZONE(L);
	}
}
#endif // ENABLE_LUA
