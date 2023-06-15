#pragma once

#pragma warning(push)
#pragma warning(disable: 26495)
#define LUABRIDGE_CXX17
#include <NativeLua/lua.hpp>
#include <LuaBridge/LuaBridge.h>
#pragma warning(pop)

#include <string>
#include <functional>

#include "utils/logger.h"
#include "utils/str.h"


using luabridge::LuaRef;
using luabridge::LuaException;


namespace lua { namespace bridge = luabridge; }


template <typename _Ty>
concept LuaValidKey = requires(const LuaRef& env, const _Ty& key) { { env[key] }; };

template <typename _Ty>
concept LuaValidValue = requires(const LuaRef& env, const char* key, const _Ty& value) { { env[key] = value }; };


class LuaEnv
{
private:
	static const LuaEnv Instance;

private:
	lua_State* _state;

public:
	LuaEnv(const LuaEnv&) = delete;
	LuaEnv(LuaEnv&&) noexcept = delete;

	LuaEnv& operator= (const LuaEnv&) = delete;
	LuaEnv& operator= (LuaEnv&&) noexcept = delete;

private:
	inline LuaEnv() : _state(luaL_newstate())
	{
		LuaException::enableExceptions(_state);
		luaL_openlibs(_state);
	}

	inline ~LuaEnv() { lua_close(_state); }

public:
	static constexpr const LuaEnv& instance() { return Instance; }

	constexpr lua_State* getState() const { return _state; }
};

inline const LuaEnv LuaEnv::Instance;


namespace lua
{
	constexpr lua_State* state() { return LuaEnv::instance().getState(); }
}

namespace lua::utils
{
	template <LuaValidValue _Ty>
	inline bridge::Result push(const _Ty& value) { return bridge::push<_Ty>(state(), value); }

	inline bridge::Result pushNil() { return push(nullptr); }

	inline void pop(unsigned int amount = 1) { lua_pop(state(), static_cast<int>(amount)); }

	inline LuaRef refFromStack(int index) { return LuaRef::fromStack(state(), index); }
	inline LuaRef refFromStack() { return LuaRef::fromStack(state()); }

	inline LuaRef popRefFromStack()
	{
		LuaRef ref = refFromStack();
		pop();
		return ref;
	}

	inline LuaRef newRef() { return LuaRef(state()); }

	template <LuaValidValue _Ty>
	inline LuaRef newRef(const _Ty& value) { return LuaRef(state(), value); }

	inline LuaRef newTableRef() { return LuaRef::newTable(state()); }

	inline void resetRef(LuaRef& ref) { ref = newRef(); }

	template <typename... _ArgsTys>
	inline void error(std::string_view fmt, _ArgsTys&&... args)
	{
		luaL_error(state(), ::utils::str::format(fmt, std::forward<_ArgsTys>(args)...).c_str());
	}

	inline void forEachInLastTable(const std::function<void(const LuaRef&, const LuaRef&)>& action)
	{
		lua_State* ls = state();
		pushNil();
		while (lua_next(ls, -2))
		{
			action(refFromStack(-2), refFromStack(-1));

			pop();
		}

		pop();
	}

	inline void forEachInTable(const LuaRef& table, const std::function<void(const LuaRef&, const LuaRef&)>& action)
	{
		table.push();
		forEachInLastTable(action);
	}

	inline void copyTableEntriesFromLastTable(const LuaRef& toTable)
	{
		forEachInLastTable([&toTable](const LuaRef& key, const LuaRef& value) {
			toTable[key] = value;
		});
	}

	inline void copyTableEntries(const LuaRef& fromTable, const LuaRef& toTable)
	{
		fromTable.push();
		copyTableEntriesFromLastTable(toTable);
	}

	inline std::string extractErrorFromStack()
	{
		lua_State* ls = state();
		if (lua_gettop(ls) >= 0)
		{
			char const* s = lua_tostring(ls, -1);
			return std::string(s ? s : "");
		}
		return std::string("missing error");
	}

	inline bool catchError(int status)
	{
		if (status != LUA_OK)
		{
			logger::error("LUA error: {}", extractErrorFromStack());
			return false;
		}
		return true;
	}

	inline bool pcall(int nargs = 0, int nresults = 0, int msgh = 0)
	{
		try
		{
			lua::bridge::pcall(state(), nargs, nresults, msgh);
			return true;
		}
		catch (const LuaException& ex)
		{
			logger::error("LUA module error: {}", ex.what());
			return false;
		}
	}

	inline LuaRef getGlobalValue(std::string_view name) { return lua::bridge::getGlobal(state(), name.data()); }

	inline void setGlobalValue(std::string_view name, const LuaRef& value) { lua::bridge::setGlobal(state(), value, name.data()); }

	inline void pushGlobal(std::string_view name) { getGlobalValue(name).push(); }

	inline lua::bridge::Namespace getGlobalNamespace() { return lua::bridge::getGlobalNamespace(state()); }

	template <LuaValidValue _Ty>
	inline void setGlobalValue(std::string_view name, const _Ty& value) { lua::bridge::setGlobal<_Ty>(state(), value, name.data()); }

	inline void setGlobalNil(std::string_view name) { lua::bridge::setGlobal(state(), nullptr, name.data()); }

	inline void setFromGlobal(std::string_view name, const LuaRef& env) { env[name] = getGlobalValue(name); }
}
