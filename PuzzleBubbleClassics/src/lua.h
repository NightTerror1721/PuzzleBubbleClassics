#pragma once

#include "common.h"

#include <lua/lua.hpp>
#include <LuaBridge/LuaBridge.h>

typedef lua_State LuaState;
typedef luabridge::LuaRef LuaRef;

namespace lua
{
	LuaState* new_state();
	void close_state(LuaState* state);

	void open_default_libs(LuaState* state);

	int load_file(LuaState* state, const char* filename, String* error_msg = nullptr);
	int load_file(LuaState* state, const String& filename, String* error_msg = nullptr);
	int load_file(LuaState* state, const Path& filename, String* error_msg = nullptr);

	int runfile(LuaState* state, const char* filename, String* error_msg = nullptr);
	int runfile(LuaState* state, const String& filename, String* error_msg = nullptr);
	int runfile(LuaState* state, const Path& filename, String* error_msg = nullptr);

	void error(LuaState* state, const char* msg);
	void error(LuaState* state, const String& msg);
}

namespace lua::meta
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

namespace lua
{
	template <typename... _ArgsTys>
	inline void safe_vcall(const LuaRef& ref, _ArgsTys&&... args)
	{
		try
		{
			ref(std::forward<_ArgsTys>(args)...);
		}
		catch (const luabridge::LuaException& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}

	template <typename... _ArgsTys>
	inline LuaRef safe_call(const LuaRef& ref, _ArgsTys&&... args)
	{
		try
		{
			ref(std::forward<_ArgsTys>(args)...);
		}
		catch (const luabridge::LuaException& ex)
		{
			std::cerr << ex.what() << std::endl;
		}
	}
}
