module;

#include "../headers.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

export module Lua:raw;

import Common;

export using LuaState = lua_State;
export using luabridge::LuaRef;
export using luabridge::LuaException;

export namespace lua
{
	inline String to_string(lua_State* state, int index)
	{
		Size len;
		const char* cstr = lua_tolstring(state, index, &len);
		return String(cstr, len);
	}

	inline bool catch_error(lua_State* state, int status)
	{
		try
		{
			if (status != LUA_OK)
				throw luabridge::LuaException(state, status);
			return true;
		}
		catch (const luabridge::LuaException& ex)
		{
			std::cerr << ex.what() << std::endl;
			return false;
		}
	}
}

export namespace lua
{
	inline lua_State* newState() { return luaL_newstate(); }
	inline void closeState(lua_State* state) { lua_close(state); }

	inline void openDefaultLibs(lua_State* state) { luaL_openlibs(state); }

	inline void error(lua_State* state, const char* msg) { luaL_error(state, msg); }
	inline void error(lua_State* state, const String& msg) { luaL_error(state, msg.c_str()); }
}

namespace lua::lib::name
{
	export constexpr const char Import[] = "import";
	export constexpr const char OpenLib[] = "openlib";
}

export namespace lua::meta
{
	constexpr const char* add = "__add";
	constexpr const char* sub = "__sub";
	constexpr const char* mul = "__mul";
	constexpr const char* div = "__div";
	constexpr const char* mod = "__mod";
	constexpr const char* pow = "__pow";
	constexpr const char* unm = "__unm";
	constexpr const char* idiv = "__idiv";
	constexpr const char* band = "__band";
	constexpr const char* bor = "__bor";
	constexpr const char* bxor = "__bxor";
	constexpr const char* bnot = "__bnot";
	constexpr const char* shl = "__shl";
	constexpr const char* shr = "__shr";
	constexpr const char* concat = "__concat";
	constexpr const char* len = "__len";
	constexpr const char* eq = "__eq";
	constexpr const char* lt = "__lt";
	constexpr const char* le = "__le";
	constexpr const char* index = "__index";
	constexpr const char* newindex = "__newindex";
	constexpr const char* call = "__call";
	constexpr const char* tostring = "__tostring";
	constexpr const char* gc = "__gc";
	constexpr const char* close = "__close";
	constexpr const char* mode = "__mode";
	constexpr const char* name = "__name";
}
