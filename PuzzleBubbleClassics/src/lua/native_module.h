#pragma once

#include "env.h"

#include "utils/logger.h"

#include <unordered_map>
#include <unordered_set>


using LuaNativeModuleConstructor = std::function<bool(lua::bridge::Namespace&, lua_State*)>;

class LuaNativeModuleManager;

class LuaNativeModule
{
public:
	friend LuaNativeModuleManager;

private:
	std::string _name;
	LuaNativeModuleConstructor _constructor;
	std::unordered_set<std::string> _dependences;

	mutable bool _built = false;

public:
	LuaNativeModule() = delete;
	LuaNativeModule(const LuaNativeModule&) = delete;
	LuaNativeModule(LuaNativeModule&&) noexcept = delete;
	~LuaNativeModule() = default;

	LuaNativeModule& operator= (const LuaNativeModule&) = delete;
	LuaNativeModule& operator= (LuaNativeModule&&) noexcept = delete;

	constexpr bool operator== (const LuaNativeModule& other) const noexcept { return _name == other._name; }
	constexpr std::strong_ordering operator<=> (const LuaNativeModule& other) const noexcept { return _name <=> other._name; }

public:
	inline LuaNativeModule(std::string_view name, const LuaNativeModuleConstructor& constructor, std::initializer_list<std::string> dependences) :
		_name(name),
		_constructor(constructor),
		_dependences(dependences)
	{}

public:
	constexpr const std::string& getName() const { return _name; }

	constexpr const LuaNativeModuleConstructor& getConstructor() const { return _constructor; }

	inline bool hasDependences() const { return !_dependences.empty(); }
	constexpr const std::unordered_set<std::string>& getDependences() const { return _dependences; }

private:
	bool open(const LuaRef& env, const std::function<bool(const LuaNativeModule&)>& condition, bool importMode) const;

public:
	inline bool includeIn(const LuaRef& env, const std::function<bool(const LuaNativeModule&)>& condition) const { return open(env, condition, false); }
	inline bool importIn(const LuaRef& env, const std::function<bool(const LuaNativeModule&)>& condition) const { return open(env, condition, false); }
};



class LuaNativeModuleManager
{
private:
	static LuaNativeModuleManager Instance;

private:
	std::unordered_map<std::string, std::shared_ptr<LuaNativeModule>> _modules;

public:
	LuaNativeModuleManager(const LuaNativeModuleManager&) = delete;
	LuaNativeModuleManager(LuaNativeModuleManager&&) noexcept = delete;

	LuaNativeModuleManager& operator= (const LuaNativeModuleManager&) = delete;
	LuaNativeModuleManager& operator= (LuaNativeModuleManager&&) noexcept = delete;

private:
	LuaNativeModuleManager() = default;
	~LuaNativeModuleManager() = default;

public:
	inline bool exists(const std::string& name) const { return _modules.contains(name); }

	std::shared_ptr<LuaNativeModule> get(const std::string& name) const;

	bool registerModule(const std::string& name, const LuaNativeModuleConstructor& constructor, std::initializer_list<std::string> dependences = {});

public:
	static inline LuaNativeModuleManager& instance() { return Instance; }
};
