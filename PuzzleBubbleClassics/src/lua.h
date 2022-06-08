#pragma once

#include "common.h"
#include "globals.h"

#include <lua/lua.hpp>
#include <LuaBridge/LuaBridge.h>

typedef lua_State LuaState;
typedef luabridge::LuaRef LuaRef;
typedef luabridge::LuaException LuaException;

class LuaScript;

namespace lua
{
	LuaState* new_state();
	void close_state(LuaState* state);

	void open_default_libs(LuaState* state);

	void error(LuaState* state, const char* msg);
	void error(LuaState* state, const String& msg);

	LuaState* get_default_state();

	LuaScript get_script(const char* filename);
	LuaScript get_script(const String& filename);
	LuaScript get_script(const Path& filepath);
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

namespace lua::lib::defs::autoimport
{
	static constexpr const char Import[] = "import";
	static constexpr const char Print[] = "print";
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




class LuaGlobalState
{
private:
	LuaState* _state;

public:
	LuaGlobalState(const LuaGlobalState&) = delete;
	LuaGlobalState(LuaGlobalState&&) noexcept = delete;

	LuaGlobalState& operator= (const LuaGlobalState&) = delete;
	LuaGlobalState& operator= (LuaGlobalState&&) noexcept = delete;

private:
	inline LuaGlobalState() : _state(lua::new_state()) {}

public:
	inline ~LuaGlobalState()
	{
		if(_state)
			lua::close_state(_state);
		_state = nullptr;
	}

	inline LuaState& state() { return *_state; }
	inline const LuaState& state() const { return *_state; }

public:
	friend GlobalsManager;
};




class LuaScriptManager;
class LuaChunk;

class LuaScriptManagerLink
{
private:
	LuaScriptManager* _manager = nullptr;
	LuaState* _state = nullptr;

private:
	LuaScriptManagerLink(LuaScriptManager& manager);

public:
	LuaScriptManagerLink() = default;
	LuaScriptManagerLink(const LuaScriptManagerLink&) = default;
	LuaScriptManagerLink(LuaScriptManagerLink&&) noexcept = default;
	~LuaScriptManagerLink() = default;

	LuaScriptManagerLink& operator= (const LuaScriptManagerLink&) = default;
	LuaScriptManagerLink& operator= (LuaScriptManagerLink&&) noexcept = default;

public:
	void pushCall(const String& chunk, const LuaRef& currentEnv) const;
	void popCall() const;

	inline LuaState* getLuaState() const { return _state; }

	inline bool isValid() const { return _state && _manager; }

public:
	inline operator bool() const { return _state && _manager; }
	inline bool operator! () const { return !_state || !_manager; }

public:
	friend LuaScriptManager;
};








class LuaChunk
{
private:
	static constexpr const char scriptIdUp[] = "__scriptid__";

	String _id;
	Path _path;
	Path _dirPath;
	LuaRef _chunk;
	LuaRef _env;
	LuaScriptManagerLink _manager;


public:
	LuaChunk() = default;
	LuaChunk(const LuaChunk&) = delete;
	LuaChunk(LuaChunk&&) noexcept = delete;
	~LuaChunk() = default;

	LuaChunk& operator= (const LuaChunk&) = delete;
	LuaChunk& operator= (LuaChunk&&) noexcept = delete;

public:
	LuaChunk(const LuaScriptManagerLink& managerLink, const Path& path);

	LuaRef* getEnv();
	const LuaRef* getEnv() const;

	void run(const LuaRef* customEnv = nullptr) const;

private:
	bool load();
	void destroy();
	static void prepareEnv(LuaState* state, const LuaRef& env);

public:
	inline const String& getId() const { return _id; }
	inline const Path& getPath() const { return _path; }
	inline const Path& getDirectory() const { return _dirPath; }
	inline LuaState* getLuaState() const { return _manager.getLuaState(); }

	inline bool isValid() const { return _manager && !_id.empty(); }
};





class LuaScript
{
private:
	std::shared_ptr<LuaChunk> _chunk;

public:
	LuaScript() = default;
	LuaScript(const LuaScript&) = default;
	LuaScript(LuaScript&&) noexcept = default;
	~LuaScript() = default;

	LuaScript& operator= (const LuaScript&) = default;
	LuaScript& operator= (LuaScript&&) noexcept = default;

public:
	inline LuaScript(const std::shared_ptr<LuaChunk>& chunk) : _chunk(chunk) {}

	inline bool isValid() const { return _chunk && _chunk->isValid(); }

	inline Path getDirectory() const { return isValid() ? _chunk->getDirectory() : Path(); }

	inline LuaRef& getEnv() { return *_chunk->getEnv(); }
	inline const LuaRef& getEnv() const { return *_chunk->getEnv(); }

	template <typename _Ty = LuaRef>
	inline _Ty getEnvValue(const char* name) const
	{
		if constexpr (std::same_as<LuaRef, _Ty>)
			return getEnv()[name];
		else
			return getEnv()[name].cast<_Ty>();
	}

	template <typename _Ty>
	inline void setEnvValue(const char* name, const _Ty& value)
	{
		getEnv()[name] = value;
	}

	inline void setEnvValueFromGlobal(const char* globalName)
	{
		if (isValid())
			setEnvValue(globalName, luabridge::getGlobal(_chunk->getLuaState(), globalName));
	}
	inline void setEnvValueFromGlobal(const String& globalName) { setEnvValueFromGlobal(globalName.c_str()); }

private:
	template <typename _Ty> requires
		std::ranges::range<_Ty> &&
		(std::convertible_to<std::ranges::range_value_t<_Ty>, const char*> || std::same_as<std::ranges::range_value_t<_Ty>, std::string>)
	void _setEnvValuesFromGlobal(const _Ty& names)
	{
		if (isValid())
		{
			LuaRef& env = *_chunk->getEnv();
			LuaState* state = _chunk->getLuaState();
			auto end = std::ranges::end(names);
			for (auto it = std::ranges::begin(names); it != end; ++it)
			{
				const char* name;
				if constexpr (std::same_as<std::ranges::range_value_t<_Ty>, std::string>)
					name = it->c_str();
				else
					name = static_cast<const char*>(*it);
				env[name] = luabridge::getGlobal(state, name);
			}
		}
	}

public:
	template <typename _Ty> requires
		std::ranges::range<_Ty> &&
		(std::convertible_to<std::ranges::range_value_t<_Ty>, const char*> || std::same_as<std::ranges::range_value_t<_Ty>, std::string>)
	inline void setEnvValuesFromGlobal(const _Ty& names)
	{
		_setEnvValuesFromGlobal<_Ty>(names);
	}

	inline void setEnvValuesFromGlobal(std::initializer_list<const char*> names)
	{
		_setEnvValuesFromGlobal(names);
	}

	inline void setEnvValuesFromGlobal(std::initializer_list<String> names)
	{
		_setEnvValuesFromGlobal(names);
	}

	template <std::same_as<std::string>... _ArgsTys>
	inline void setEnvValuesFromGlobal(_ArgsTys&&... names)
	{
		setEnvValuesFromGlobal(std::initializer_list<String>{ std::forward<_ArgsTys>(names)... });
	}

	template <std::convertible_to<const char*>... _ArgsTys>
	inline void setEnvValuesFromGlobal(_ArgsTys&&... names)
	{
		setEnvValuesFromGlobal(std::initializer_list<const char*>{ std::forward<_ArgsTys>(names)... });
	}

public:
	inline void operator() () const
	{
		if (_chunk)
			_chunk->run();
	}

	inline void operator() (const LuaRef& customEnv) const
	{
		if (_chunk)
			_chunk->run(std::addressof(customEnv));
	}

	inline operator bool() const { return _chunk && _chunk->isValid(); }
	inline bool operator! () const { return !_chunk || !_chunk->isValid(); }

public:
	class EnvPair
	{
	private:
		LuaScript& _scr;
		const char* const _name;

		inline EnvPair(LuaScript& scr, const char* name) : _scr(scr), _name(name) {}

	public:
		template <typename _Ty>
		inline const _Ty& operator= (const _Ty& value) { return _scr.setEnvValue(_name, value), value; }

		template <typename _Ty>
		inline _Ty& operator= (_Ty& value) { return _scr.setEnvValue(_name, value), value; }

		template <typename _Ty = LuaRef>
		inline operator _Ty() const { return _scr.getEnvValue<_Ty>(_name); }

		friend LuaScript;
	};

	class ConstEnvPair
	{
	private:
		const LuaScript& _scr;
		const char* const _name;

		inline ConstEnvPair(const LuaScript& scr, const char* name) : _scr(scr), _name(name) {}

	public:
		template <typename _Ty = LuaRef>
		inline operator _Ty() const { return _scr.getEnvValue<_Ty>(_name); }

		friend LuaScript;
	};

	inline EnvPair operator[] (const char* name) { return EnvPair(*this, name); }
	inline ConstEnvPair operator[] (const char* name) const { return ConstEnvPair(*this, name); }
};







class LuaScriptManager
{
private:
	struct CallInfo
	{
		String scriptId;
		const LuaRef* currentEnv;
	};

private:
	LuaState* _state;
	std::unordered_map<String, std::shared_ptr<LuaChunk>> _chunks;
	std::stack<CallInfo> _callStack;

private:
	LuaScriptManager();

public:
	LuaScriptManager(const LuaScriptManager&) = delete;
	LuaScriptManager(LuaScriptManager&&) = delete;

	LuaScriptManager& operator= (const LuaScriptManager&) = delete;
	LuaScriptManager& operator= (LuaScriptManager&&) noexcept = delete;

private:
	~LuaScriptManager();

	std::shared_ptr<LuaChunk> findChunk(const Path& path);

public:
	LuaScript getScript(const Path& path);

	LuaScript getCurrentRunScript() const;

	const LuaRef* getCurrentRunScriptEnv() const;

public:
	inline LuaState* getState() const { return _state; }

	inline LuaScript getScript(const String& path) { return getScript(Path(path)); }
	inline LuaScript getScript(const char* path) { return getScript(Path(path)); }

public:
	friend LuaScriptManagerLink;
	friend GlobalsManager;
};







inline LuaScriptManagerLink::LuaScriptManagerLink(LuaScriptManager& manager) : _manager(&manager), _state(manager._state) {}

inline void LuaScriptManagerLink::pushCall(const String& chunkId, const LuaRef& currentEnv) const
{
	if(_manager)
		_manager->_callStack.push({ chunkId, std::addressof(currentEnv) });
}

inline void LuaScriptManagerLink::popCall() const
{
	if (_manager)
		_manager->_callStack.pop();
}
