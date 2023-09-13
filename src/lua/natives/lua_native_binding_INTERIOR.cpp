#if defined (ENABLE_LUA)

#include "lua_native_binding.hpp"
#include "natives.hpp"

namespace lua::native
{
	static float LUA_NATIVE_INTERIOR_GET_INTERIOR_HEADING(Interior interior)
	{
		auto retval = INTERIOR::GET_INTERIOR_HEADING(interior);
		return retval;
	}

	static std::tuple<Vector3, Hash> LUA_NATIVE_INTERIOR_GET_INTERIOR_LOCATION_AND_NAMEHASH(Interior interior, Vector3 position, Hash nameHash)
	{
		std::tuple<Vector3, Hash> return_values;
		INTERIOR::GET_INTERIOR_LOCATION_AND_NAMEHASH(interior, &position, &nameHash);
		std::get<0>(return_values) = position;
		std::get<1>(return_values) = nameHash;

		return return_values;
	}

	static int LUA_NATIVE_INTERIOR_GET_INTERIOR_GROUP_ID(Interior interior)
	{
		auto retval = INTERIOR::GET_INTERIOR_GROUP_ID(interior);
		return retval;
	}

	static Vector3 LUA_NATIVE_INTERIOR_GET_OFFSET_FROM_INTERIOR_IN_WORLD_COORDS(Interior interior, float x, float y, float z)
	{
		auto retval = INTERIOR::GET_OFFSET_FROM_INTERIOR_IN_WORLD_COORDS(interior, x, y, z);
		return retval;
	}

	static bool LUA_NATIVE_INTERIOR_IS_INTERIOR_SCENE()
	{
		auto retval = (bool)INTERIOR::IS_INTERIOR_SCENE();
		return retval;
	}

	static bool LUA_NATIVE_INTERIOR_IS_VALID_INTERIOR(Interior interior)
	{
		auto retval = (bool)INTERIOR::IS_VALID_INTERIOR(interior);
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_CLEAR_ROOM_FOR_ENTITY(Entity entity)
	{
		INTERIOR::CLEAR_ROOM_FOR_ENTITY(entity);
	}

	static void LUA_NATIVE_INTERIOR_FORCE_ROOM_FOR_ENTITY(Entity entity, Interior interior, Hash roomHashKey)
	{
		INTERIOR::FORCE_ROOM_FOR_ENTITY(entity, interior, roomHashKey);
	}

	static Hash LUA_NATIVE_INTERIOR_GET_ROOM_KEY_FROM_ENTITY(Entity entity)
	{
		auto retval = INTERIOR::GET_ROOM_KEY_FROM_ENTITY(entity);
		return retval;
	}

	static Hash LUA_NATIVE_INTERIOR_GET_KEY_FOR_ENTITY_IN_ROOM(Entity entity)
	{
		auto retval = INTERIOR::GET_KEY_FOR_ENTITY_IN_ROOM(entity);
		return retval;
	}

	static Interior LUA_NATIVE_INTERIOR_GET_INTERIOR_FROM_ENTITY(Entity entity)
	{
		auto retval = INTERIOR::GET_INTERIOR_FROM_ENTITY(entity);
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_RETAIN_ENTITY_IN_INTERIOR(Entity entity, Interior interior)
	{
		INTERIOR::RETAIN_ENTITY_IN_INTERIOR(entity, interior);
	}

	static void LUA_NATIVE_INTERIOR_CLEAR_INTERIOR_STATE_OF_ENTITY(Entity entity)
	{
		INTERIOR::CLEAR_INTERIOR_STATE_OF_ENTITY(entity);
	}

	static void LUA_NATIVE_INTERIOR_FORCE_ACTIVATING_TRACKING_ON_ENTITY(Any p0, Any p1)
	{
		INTERIOR::FORCE_ACTIVATING_TRACKING_ON_ENTITY(p0, p1);
	}

	static void LUA_NATIVE_INTERIOR_FORCE_ROOM_FOR_GAME_VIEWPORT(int interiorID, Hash roomHashKey)
	{
		INTERIOR::FORCE_ROOM_FOR_GAME_VIEWPORT(interiorID, roomHashKey);
	}

	static void LUA_NATIVE_INTERIOR_SET_ROOM_FOR_GAME_VIEWPORT_BY_NAME(const char* roomName)
	{
		INTERIOR::SET_ROOM_FOR_GAME_VIEWPORT_BY_NAME(roomName);
	}

	static void LUA_NATIVE_INTERIOR_SET_ROOM_FOR_GAME_VIEWPORT_BY_KEY(Hash roomHashKey)
	{
		INTERIOR::SET_ROOM_FOR_GAME_VIEWPORT_BY_KEY(roomHashKey);
	}

	static Hash LUA_NATIVE_INTERIOR_GET_ROOM_KEY_FOR_GAME_VIEWPORT()
	{
		auto retval = INTERIOR::GET_ROOM_KEY_FOR_GAME_VIEWPORT();
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_CLEAR_ROOM_FOR_GAME_VIEWPORT()
	{
		INTERIOR::CLEAR_ROOM_FOR_GAME_VIEWPORT();
	}

	static Interior LUA_NATIVE_INTERIOR_GET_INTERIOR_FROM_PRIMARY_VIEW()
	{
		auto retval = INTERIOR::GET_INTERIOR_FROM_PRIMARY_VIEW();
		return retval;
	}

	static Interior LUA_NATIVE_INTERIOR_GET_INTERIOR_AT_COORDS(float x, float y, float z)
	{
		auto retval = INTERIOR::GET_INTERIOR_AT_COORDS(x, y, z);
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_ADD_PICKUP_TO_INTERIOR_ROOM_BY_NAME(Pickup pickup, const char* roomName)
	{
		INTERIOR::ADD_PICKUP_TO_INTERIOR_ROOM_BY_NAME(pickup, roomName);
	}

	static void LUA_NATIVE_INTERIOR_PIN_INTERIOR_IN_MEMORY(Interior interior)
	{
		INTERIOR::PIN_INTERIOR_IN_MEMORY(interior);
	}

	static void LUA_NATIVE_INTERIOR_UNPIN_INTERIOR(Interior interior)
	{
		INTERIOR::UNPIN_INTERIOR(interior);
	}

	static bool LUA_NATIVE_INTERIOR_IS_INTERIOR_READY(Interior interior)
	{
		auto retval = (bool)INTERIOR::IS_INTERIOR_READY(interior);
		return retval;
	}

	static bool LUA_NATIVE_INTERIOR_SET_INTERIOR_IN_USE(Interior interior)
	{
		auto retval = (bool)INTERIOR::SET_INTERIOR_IN_USE(interior);
		return retval;
	}

	static Interior LUA_NATIVE_INTERIOR_GET_INTERIOR_AT_COORDS_WITH_TYPE(float x, float y, float z, const char* interiorType)
	{
		auto retval = INTERIOR::GET_INTERIOR_AT_COORDS_WITH_TYPE(x, y, z, interiorType);
		return retval;
	}

	static Interior LUA_NATIVE_INTERIOR_GET_INTERIOR_AT_COORDS_WITH_TYPEHASH(float x, float y, float z, Hash typeHash)
	{
		auto retval = INTERIOR::GET_INTERIOR_AT_COORDS_WITH_TYPEHASH(x, y, z, typeHash);
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_ACTIVATE_INTERIOR_GROUPS_USING_CAMERA()
	{
		INTERIOR::ACTIVATE_INTERIOR_GROUPS_USING_CAMERA();
	}

	static bool LUA_NATIVE_INTERIOR_IS_COLLISION_MARKED_OUTSIDE(float x, float y, float z)
	{
		auto retval = (bool)INTERIOR::IS_COLLISION_MARKED_OUTSIDE(x, y, z);
		return retval;
	}

	static Interior LUA_NATIVE_INTERIOR_GET_INTERIOR_FROM_COLLISION(float x, float y, float z)
	{
		auto retval = INTERIOR::GET_INTERIOR_FROM_COLLISION(x, y, z);
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_ENABLE_STADIUM_PROBES_THIS_FRAME(bool toggle)
	{
		INTERIOR::ENABLE_STADIUM_PROBES_THIS_FRAME(toggle);
	}

	static void LUA_NATIVE_INTERIOR_ACTIVATE_INTERIOR_ENTITY_SET(Interior interior, const char* entitySetName)
	{
		INTERIOR::ACTIVATE_INTERIOR_ENTITY_SET(interior, entitySetName);
	}

	static void LUA_NATIVE_INTERIOR_DEACTIVATE_INTERIOR_ENTITY_SET(Interior interior, const char* entitySetName)
	{
		INTERIOR::DEACTIVATE_INTERIOR_ENTITY_SET(interior, entitySetName);
	}

	static bool LUA_NATIVE_INTERIOR_IS_INTERIOR_ENTITY_SET_ACTIVE(Interior interior, const char* entitySetName)
	{
		auto retval = (bool)INTERIOR::IS_INTERIOR_ENTITY_SET_ACTIVE(interior, entitySetName);
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_SET_INTERIOR_ENTITY_SET_TINT_INDEX(Interior interior, const char* entitySetName, int color)
	{
		INTERIOR::SET_INTERIOR_ENTITY_SET_TINT_INDEX(interior, entitySetName, color);
	}

	static void LUA_NATIVE_INTERIOR_REFRESH_INTERIOR(Interior interior)
	{
		INTERIOR::REFRESH_INTERIOR(interior);
	}

	static void LUA_NATIVE_INTERIOR_ENABLE_EXTERIOR_CULL_MODEL_THIS_FRAME(Hash mapObjectHash)
	{
		INTERIOR::ENABLE_EXTERIOR_CULL_MODEL_THIS_FRAME(mapObjectHash);
	}

	static void LUA_NATIVE_INTERIOR_ENABLE_SHADOW_CULL_MODEL_THIS_FRAME(Hash mapObjectHash)
	{
		INTERIOR::ENABLE_SHADOW_CULL_MODEL_THIS_FRAME(mapObjectHash);
	}

	static void LUA_NATIVE_INTERIOR_DISABLE_INTERIOR(Interior interior, bool toggle)
	{
		INTERIOR::DISABLE_INTERIOR(interior, toggle);
	}

	static bool LUA_NATIVE_INTERIOR_IS_INTERIOR_DISABLED(Interior interior)
	{
		auto retval = (bool)INTERIOR::IS_INTERIOR_DISABLED(interior);
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_CAP_INTERIOR(Interior interior, bool toggle)
	{
		INTERIOR::CAP_INTERIOR(interior, toggle);
	}

	static bool LUA_NATIVE_INTERIOR_IS_INTERIOR_CAPPED(Interior interior)
	{
		auto retval = (bool)INTERIOR::IS_INTERIOR_CAPPED(interior);
		return retval;
	}

	static void LUA_NATIVE_INTERIOR_DISABLE_METRO_SYSTEM(bool toggle)
	{
		INTERIOR::DISABLE_METRO_SYSTEM(toggle);
	}

	static void LUA_NATIVE_INTERIOR_SET_IS_EXTERIOR_ONLY(Entity entity, bool toggle)
	{
		INTERIOR::SET_IS_EXTERIOR_ONLY(entity, toggle);
	}

	void init_native_binding_INTERIOR(sol::state& L)
	{
		auto INTERIOR = L["INTERIOR"].get_or_create<sol::table>();
		INTERIOR.set_function("GET_INTERIOR_HEADING", LUA_NATIVE_INTERIOR_GET_INTERIOR_HEADING);
		INTERIOR.set_function("GET_INTERIOR_LOCATION_AND_NAMEHASH", LUA_NATIVE_INTERIOR_GET_INTERIOR_LOCATION_AND_NAMEHASH);
		INTERIOR.set_function("GET_INTERIOR_GROUP_ID", LUA_NATIVE_INTERIOR_GET_INTERIOR_GROUP_ID);
		INTERIOR.set_function("GET_OFFSET_FROM_INTERIOR_IN_WORLD_COORDS", LUA_NATIVE_INTERIOR_GET_OFFSET_FROM_INTERIOR_IN_WORLD_COORDS);
		INTERIOR.set_function("IS_INTERIOR_SCENE", LUA_NATIVE_INTERIOR_IS_INTERIOR_SCENE);
		INTERIOR.set_function("IS_VALID_INTERIOR", LUA_NATIVE_INTERIOR_IS_VALID_INTERIOR);
		INTERIOR.set_function("CLEAR_ROOM_FOR_ENTITY", LUA_NATIVE_INTERIOR_CLEAR_ROOM_FOR_ENTITY);
		INTERIOR.set_function("FORCE_ROOM_FOR_ENTITY", LUA_NATIVE_INTERIOR_FORCE_ROOM_FOR_ENTITY);
		INTERIOR.set_function("GET_ROOM_KEY_FROM_ENTITY", LUA_NATIVE_INTERIOR_GET_ROOM_KEY_FROM_ENTITY);
		INTERIOR.set_function("GET_KEY_FOR_ENTITY_IN_ROOM", LUA_NATIVE_INTERIOR_GET_KEY_FOR_ENTITY_IN_ROOM);
		INTERIOR.set_function("GET_INTERIOR_FROM_ENTITY", LUA_NATIVE_INTERIOR_GET_INTERIOR_FROM_ENTITY);
		INTERIOR.set_function("RETAIN_ENTITY_IN_INTERIOR", LUA_NATIVE_INTERIOR_RETAIN_ENTITY_IN_INTERIOR);
		INTERIOR.set_function("CLEAR_INTERIOR_STATE_OF_ENTITY", LUA_NATIVE_INTERIOR_CLEAR_INTERIOR_STATE_OF_ENTITY);
		INTERIOR.set_function("FORCE_ACTIVATING_TRACKING_ON_ENTITY", LUA_NATIVE_INTERIOR_FORCE_ACTIVATING_TRACKING_ON_ENTITY);
		INTERIOR.set_function("FORCE_ROOM_FOR_GAME_VIEWPORT", LUA_NATIVE_INTERIOR_FORCE_ROOM_FOR_GAME_VIEWPORT);
		INTERIOR.set_function("SET_ROOM_FOR_GAME_VIEWPORT_BY_NAME", LUA_NATIVE_INTERIOR_SET_ROOM_FOR_GAME_VIEWPORT_BY_NAME);
		INTERIOR.set_function("SET_ROOM_FOR_GAME_VIEWPORT_BY_KEY", LUA_NATIVE_INTERIOR_SET_ROOM_FOR_GAME_VIEWPORT_BY_KEY);
		INTERIOR.set_function("GET_ROOM_KEY_FOR_GAME_VIEWPORT", LUA_NATIVE_INTERIOR_GET_ROOM_KEY_FOR_GAME_VIEWPORT);
		INTERIOR.set_function("CLEAR_ROOM_FOR_GAME_VIEWPORT", LUA_NATIVE_INTERIOR_CLEAR_ROOM_FOR_GAME_VIEWPORT);
		INTERIOR.set_function("GET_INTERIOR_FROM_PRIMARY_VIEW", LUA_NATIVE_INTERIOR_GET_INTERIOR_FROM_PRIMARY_VIEW);
		INTERIOR.set_function("GET_INTERIOR_AT_COORDS", LUA_NATIVE_INTERIOR_GET_INTERIOR_AT_COORDS);
		INTERIOR.set_function("ADD_PICKUP_TO_INTERIOR_ROOM_BY_NAME", LUA_NATIVE_INTERIOR_ADD_PICKUP_TO_INTERIOR_ROOM_BY_NAME);
		INTERIOR.set_function("PIN_INTERIOR_IN_MEMORY", LUA_NATIVE_INTERIOR_PIN_INTERIOR_IN_MEMORY);
		INTERIOR.set_function("UNPIN_INTERIOR", LUA_NATIVE_INTERIOR_UNPIN_INTERIOR);
		INTERIOR.set_function("IS_INTERIOR_READY", LUA_NATIVE_INTERIOR_IS_INTERIOR_READY);
		INTERIOR.set_function("SET_INTERIOR_IN_USE", LUA_NATIVE_INTERIOR_SET_INTERIOR_IN_USE);
		INTERIOR.set_function("GET_INTERIOR_AT_COORDS_WITH_TYPE", LUA_NATIVE_INTERIOR_GET_INTERIOR_AT_COORDS_WITH_TYPE);
		INTERIOR.set_function("GET_INTERIOR_AT_COORDS_WITH_TYPEHASH", LUA_NATIVE_INTERIOR_GET_INTERIOR_AT_COORDS_WITH_TYPEHASH);
		INTERIOR.set_function("ACTIVATE_INTERIOR_GROUPS_USING_CAMERA", LUA_NATIVE_INTERIOR_ACTIVATE_INTERIOR_GROUPS_USING_CAMERA);
		INTERIOR.set_function("IS_COLLISION_MARKED_OUTSIDE", LUA_NATIVE_INTERIOR_IS_COLLISION_MARKED_OUTSIDE);
		INTERIOR.set_function("GET_INTERIOR_FROM_COLLISION", LUA_NATIVE_INTERIOR_GET_INTERIOR_FROM_COLLISION);
		INTERIOR.set_function("ENABLE_STADIUM_PROBES_THIS_FRAME", LUA_NATIVE_INTERIOR_ENABLE_STADIUM_PROBES_THIS_FRAME);
		INTERIOR.set_function("ACTIVATE_INTERIOR_ENTITY_SET", LUA_NATIVE_INTERIOR_ACTIVATE_INTERIOR_ENTITY_SET);
		INTERIOR.set_function("DEACTIVATE_INTERIOR_ENTITY_SET", LUA_NATIVE_INTERIOR_DEACTIVATE_INTERIOR_ENTITY_SET);
		INTERIOR.set_function("IS_INTERIOR_ENTITY_SET_ACTIVE", LUA_NATIVE_INTERIOR_IS_INTERIOR_ENTITY_SET_ACTIVE);
		INTERIOR.set_function("SET_INTERIOR_ENTITY_SET_TINT_INDEX", LUA_NATIVE_INTERIOR_SET_INTERIOR_ENTITY_SET_TINT_INDEX);
		INTERIOR.set_function("REFRESH_INTERIOR", LUA_NATIVE_INTERIOR_REFRESH_INTERIOR);
		INTERIOR.set_function("ENABLE_EXTERIOR_CULL_MODEL_THIS_FRAME", LUA_NATIVE_INTERIOR_ENABLE_EXTERIOR_CULL_MODEL_THIS_FRAME);
		INTERIOR.set_function("ENABLE_SHADOW_CULL_MODEL_THIS_FRAME", LUA_NATIVE_INTERIOR_ENABLE_SHADOW_CULL_MODEL_THIS_FRAME);
		INTERIOR.set_function("DISABLE_INTERIOR", LUA_NATIVE_INTERIOR_DISABLE_INTERIOR);
		INTERIOR.set_function("IS_INTERIOR_DISABLED", LUA_NATIVE_INTERIOR_IS_INTERIOR_DISABLED);
		INTERIOR.set_function("CAP_INTERIOR", LUA_NATIVE_INTERIOR_CAP_INTERIOR);
		INTERIOR.set_function("IS_INTERIOR_CAPPED", LUA_NATIVE_INTERIOR_IS_INTERIOR_CAPPED);
		INTERIOR.set_function("DISABLE_METRO_SYSTEM", LUA_NATIVE_INTERIOR_DISABLE_METRO_SYSTEM);
		INTERIOR.set_function("SET_IS_EXTERIOR_ONLY", LUA_NATIVE_INTERIOR_SET_IS_EXTERIOR_ONLY);
	}
}

#endif // ENABLE_LUA
