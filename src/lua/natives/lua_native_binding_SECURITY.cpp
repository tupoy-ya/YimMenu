#if defined (ENABLE_LUA)

#include "lua_native_binding.hpp"
#include "natives.hpp"

namespace lua::native
{
	static Any LUA_NATIVE_SECURITY_REGISTER_SCRIPT_VARIABLE(Any variable)
	{
		SECURITY::REGISTER_SCRIPT_VARIABLE(&variable);
		return variable;
	}

	static Any LUA_NATIVE_SECURITY_UNREGISTER_SCRIPT_VARIABLE(Any variable)
	{
		SECURITY::UNREGISTER_SCRIPT_VARIABLE(&variable);
		return variable;
	}

	static void LUA_NATIVE_SECURITY_FORCE_CHECK_SCRIPT_VARIABLES()
	{
		SECURITY::FORCE_CHECK_SCRIPT_VARIABLES();
	}

	void init_native_binding_SECURITY(sol::state& L)
	{
		auto SECURITY = L["SECURITY"].get_or_create<sol::table>();
		SECURITY.set_function("REGISTER_SCRIPT_VARIABLE", LUA_NATIVE_SECURITY_REGISTER_SCRIPT_VARIABLE);
		SECURITY.set_function("UNREGISTER_SCRIPT_VARIABLE", LUA_NATIVE_SECURITY_UNREGISTER_SCRIPT_VARIABLE);
		SECURITY.set_function("FORCE_CHECK_SCRIPT_VARIABLES", LUA_NATIVE_SECURITY_FORCE_CHECK_SCRIPT_VARIABLES);
	}
}

#endif // ENABLE_LUA
