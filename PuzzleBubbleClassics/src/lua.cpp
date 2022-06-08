#include "lua.h"

namespace lua
{
	static String to_string(LuaState* state, int index)
	{
		Size len;
		const char* cstr = lua_tolstring(state, index, &len);
		return String(cstr, len);
	}

	static inline bool catch_error(LuaState* state, int status)
	{
		try
		{
			if (status != LUA_OK)
				throw LuaException(state, status);
			return true;
		}
		catch (const LuaException& ex)
		{
			std::cerr << ex.what() << std::endl;
			return false;
		}
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


	void error(LuaState* state, const char* msg) { luaL_error(state, msg); }
	void error(LuaState* state, const String& msg) { luaL_error(state, msg.c_str()); }


	LuaState* get_default_state() { return globals::luaScripts().getState(); }


	LuaScript get_script(const char* filename) { return globals::luaScripts().getScript(filename); }
	LuaScript get_script(const String& filename) { return globals::luaScripts().getScript(filename); }
	LuaScript get_script(const Path& filepath) { return globals::luaScripts().getScript(filepath); }
}







LuaChunk::LuaChunk(const LuaScriptManagerLink& managerLink, const Path& path) :
	_manager(managerLink),
	_path(std::filesystem::absolute(path)),
	_chunk(managerLink.getLuaState()),
	_env(managerLink.getLuaState()),
	_id(),
	_dirPath()
{
	_dirPath = Path(_path).remove_filename();
	_id = _path.string();
	load();
}

bool LuaChunk::load()
{
	auto state = _manager.getLuaState();
	bool status = lua::catch_error(state, luaL_loadfile(state, _path.string().c_str()));
	if (status)
	{
		_chunk = LuaRef::fromStack(state);
		_env = LuaRef::newTable(state);
		lua_pop(state, 1);

		return true;
	}
	else
	{
		destroy();
		return false;
	}
}

void LuaChunk::destroy()
{
	_manager = LuaScriptManagerLink();
	_path = Path();
	_dirPath = Path();
	_id = String();
}

LuaRef* LuaChunk::getEnv()
{
	if (isValid())
		return std::addressof(_env);
	return nullptr;
}

const LuaRef* LuaChunk::getEnv() const
{
	if (isValid())
		return std::addressof(_env);
	return nullptr;
}

void LuaChunk::run(const LuaRef* customEnv) const
{
	if (isValid())
	{
		const LuaRef& env = customEnv ? *customEnv : _env;
		auto state = _manager.getLuaState();

		prepareEnv(state, env);

		_chunk.push();
		env.push();
		lua_setupvalue(state, -2, 1);

		_manager.pushCall(_id, env);
		lua::catch_error(state, lua_pcall(state, 0, LUA_MULTRET, 0));
		_manager.popCall();
	}
}

void LuaChunk::prepareEnv(LuaState* state, const LuaRef& env)
{
	//env[lua::lib::defs::autoimport::Print] = luabridge::getGlobal(state, lua::lib::defs::autoimport::Print);
	env[lua::lib::defs::autoimport::Import] = luabridge::getGlobal(state, lua::lib::defs::autoimport::Import);
}







LuaScriptManager::LuaScriptManager() :
	_state(&globals::luaGlobalState().state()),
	_chunks(),
	_callStack()
{
	//lua::open_default_libs(_state);
}

LuaScriptManager::~LuaScriptManager() {}

std::shared_ptr<LuaChunk> LuaScriptManager::findChunk(const Path& path)
{
	Path absPath = std::filesystem::absolute(path);
	auto it = _chunks.find(absPath.string());
	if (it != _chunks.end())
		return it->second;
	
	std::shared_ptr<LuaChunk> chunk = std::make_shared<LuaChunk>(LuaScriptManagerLink(*this), absPath);
	if (chunk->isValid())
	{
		_chunks.insert({ chunk->getId(), chunk });
		return chunk;
	}

	return nullptr;
}

LuaScript LuaScriptManager::getScript(const Path& path)
{
	auto chunk = findChunk(path);
	if (chunk)
		return LuaScript(chunk);
	return LuaScript();
}

LuaScript LuaScriptManager::getCurrentRunScript() const
{
	if (!_callStack.empty())
	{
		const auto& info = _callStack.top();
		auto it = _chunks.find(info.scriptId);
		if (it != _chunks.end())
			return LuaScript(it->second);
	}
	return LuaScript();
}

const LuaRef* LuaScriptManager::getCurrentRunScriptEnv() const
{
	if (!_callStack.empty())
	{
		const auto& link = _callStack.top();
		return link.currentEnv;
	}
	return nullptr;
}
