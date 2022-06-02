#include "lua.h"

namespace lua
{
	static String to_string(LuaState* state, int index)
	{
		Size len;
		const char* cstr = lua_tolstring(state, index, &len);
		return String(cstr, len);
	}

	static int catch_error(LuaState* state, int status, String* error_msg)
	{
		if (status != LUA_OK)
		{
			if (error_msg)
			{
				*error_msg = to_string(state, -1);
				std::cerr << *error_msg << std::endl;
			}
			else
			{
				std::cerr << lua_tostring(state, -1) << std::endl;
			}
		}
		return status;
	}
}

namespace lua
{
	LuaState* new_state() { return luaL_newstate(); }

	void close_state(LuaState* state) { lua_close(state); }

	void open_default_libs(LuaState* state)
	{
		luaL_openlibs(state);
	}



	

	int load_file(LuaState* state, const char* filename, String* error_msg)
	{
		return catch_error(state, luaL_loadfile(state, filename), error_msg);
	}

	int load_file(LuaState* state, const String& filename, String* error_msg)
	{
		return catch_error(state, luaL_loadfile(state, filename.c_str()), error_msg);
	}

	int load_file(LuaState* state, const Path& filename, String* error_msg)
	{
		return catch_error(state, luaL_loadfile(state, filename.string().c_str()), error_msg);
	}

	int runfile(LuaState* state, const char* filename, String* error_msg)
	{
		int status = load_file(state, filename, error_msg);
		if (status == LUA_OK)
			status = catch_error(state, lua_pcall(state, 0, LUA_MULTRET, 0), error_msg);
		return status;
	}

	int runfile(LuaState* state, const String& filename, String* error_msg)
	{
		int status = load_file(state, filename, error_msg);
		if (status == LUA_OK)
			status = catch_error(state, lua_pcall(state, 0, LUA_MULTRET, 0), error_msg);
		return status;
	}

	int runfile(LuaState* state, const Path& filename, String* error_msg)
	{
		int status = load_file(state, filename, error_msg);
		if (status == LUA_OK)
			status = catch_error(state, lua_pcall(state, 0, LUA_MULTRET, 0), error_msg);
		return status;
	}


	void error(LuaState* state, const char* msg) { luaL_error(state, msg); }
	void error(LuaState* state, const String& msg) { luaL_error(state, msg.c_str()); }
}
