module;

#include "../headers.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

export module Lua:scripts;
import :raw;

import Common;
import Globals;


export class LuaGlobalState;
export class LuaChunk;
export class LuaScript;
export class LuaScriptManager;
export class LuaScriptManagerLink;



namespace globals
{
	export LuaGlobalState& luaGlobalState();
	export LuaScriptManager& luaScripts();
}

export class LuaGlobalState
{
private:
	LuaState* _state = nullptr;

public:
	LuaGlobalState(const LuaGlobalState&) = delete;
	LuaGlobalState(LuaGlobalState&&) noexcept = delete;

	LuaGlobalState& operator= (const LuaGlobalState&) = delete;
	LuaGlobalState& operator= (LuaGlobalState&&) noexcept = delete;

public:
	LuaGlobalState() {}

	inline ~LuaGlobalState()
	{
		if (_state)
			lua::closeState(_state);
		_state = nullptr;
	}

	inline LuaState& state() { return *_state; }
	inline const LuaState& state() const { return *_state; }

private:
	static globals::PriorityElementIndex _instanceIndex;

public:
	friend LuaGlobalState& ::globals::luaGlobalState();
};



export class LuaScriptManagerLink
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

	inline Path getPath() const { return isValid() ? _chunk->getPath() : Path(); }

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
		void _setEnvValuesFromGlobal(const _Ty& names) const
	{
		if (isValid())
		{
			const LuaRef& env = *_chunk->getEnv();
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

	inline void setEnvValuesFromGlobal(std::initializer_list<const char*> names) const
	{
		_setEnvValuesFromGlobal(names);
	}

	inline void setEnvValuesFromGlobal(std::initializer_list<String> names) const
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



export class LuaScriptManager
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

public:
	
	LuaScriptManager(const LuaScriptManager&) = delete;
	LuaScriptManager(LuaScriptManager&&) = delete;

	LuaScriptManager& operator= (const LuaScriptManager&) = delete;
	LuaScriptManager& operator= (LuaScriptManager&&) noexcept = delete;

public:
	LuaScriptManager();
	~LuaScriptManager();

private:
	std::shared_ptr<LuaChunk> findChunk(const Path& path);

public:
	LuaScript getScript(const Path& path);

	LuaScript getCurrentRunScript() const;

	const LuaRef* getCurrentRunScriptEnv() const;

public:
	inline LuaState* getState() const { return _state; }

	inline LuaScript getScript(const String& path) { return getScript(Path(path)); }
	inline LuaScript getScript(const char* path) { return getScript(Path(path)); }

private:
	static globals::PriorityElementIndex _instanceIndex;

public:
	friend LuaScriptManagerLink;
	friend LuaScriptManager& ::globals::luaScripts();
};



inline globals::PriorityElementIndex LuaGlobalState::_instanceIndex = globals::createPriorityElement<LuaGlobalState>(0);
inline globals::PriorityElementIndex LuaScriptManager::_instanceIndex = globals::createPriorityElement<LuaScriptManager>(1);

namespace globals
{
	export inline LuaGlobalState& luaGlobalState() { return getPriorityElementData<LuaGlobalState>(LuaGlobalState::_instanceIndex); }
	export inline LuaScriptManager& luaScripts() { return getPriorityElementData<LuaScriptManager>(LuaScriptManager::_instanceIndex); }
}



inline LuaScriptManagerLink::LuaScriptManagerLink(LuaScriptManager& manager) : _manager(&manager), _state(manager._state) {}

inline void LuaScriptManagerLink::pushCall(const String& chunkId, const LuaRef& currentEnv) const
{
	if (_manager)
		_manager->_callStack.push({ chunkId, std::addressof(currentEnv) });
}

inline void LuaScriptManagerLink::popCall() const
{
	if (_manager)
		_manager->_callStack.pop();
}
