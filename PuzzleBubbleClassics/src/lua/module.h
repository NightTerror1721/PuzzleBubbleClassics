#pragma once

#include "env.h"

#include "utils/reference.h"
#include "utils/path.h"

#include <stack>
#include <unordered_set>


class LuaModule;
class LuaModuleImportManager;

class LuaCallStack
{
public:
	friend LuaModule;

private:
	static LuaCallStack Instance;

private:
	std::stack<Reference<LuaModule>> _stack;

private:
	LuaCallStack() = default;
	~LuaCallStack() = default;

public:
	LuaCallStack(const LuaCallStack&) = delete;
	LuaCallStack(LuaCallStack&&) noexcept = delete;

	LuaCallStack& operator= (const LuaCallStack&) = delete;
	LuaCallStack& operator= (LuaCallStack&&) noexcept = delete;

private:
	static inline void push(LuaModule& mod) { Instance._stack.push(std::addressof(mod)); }
	static inline void pop() { Instance._stack.pop(); }
	static inline Reference<LuaModule> top() { return Instance._stack.empty() ? nullptr : Reference<LuaModule>(Instance._stack.top()); }
};


class LuaModule
{
public:
	friend LuaModuleImportManager;

private:
	Reference<LuaModule> _parent;
	std::shared_ptr<LuaRef> _env;

	Path _path;
	Path _directory;

	std::shared_ptr<std::unordered_set<std::string>> _importedNativeModules;
	std::shared_ptr<std::unordered_set<std::string>> _includedNativeModules;
	std::shared_ptr<std::unordered_set<Path>> _importedModules;
	std::shared_ptr<std::unordered_set<Path>> _includedModules;

	LuaRef _chunk;
	bool _loaded;

public:
	LuaModule(const LuaModule&) = delete;
	LuaModule(LuaModule&&) noexcept = delete;

	LuaModule& operator= (const LuaModule&) = delete;
	LuaModule& operator= (LuaModule&&) noexcept = delete;

private:
	LuaModule(const Path& path, Reference<LuaModule> parent);

	Path findRelativePath(const std::string_view& path);
	Path findPathFromRoot(const std::string_view& path);
	void loadBuiltinElements();
	bool run();

	bool importModule(const std::string& path, bool fromRoot);
	bool includeModule(const std::string& path, bool fromRoot);

public:
	~LuaModule() { release(); }

public:
	bool load();
	void release();

public:
	constexpr bool isLoaded() const { return _loaded; }

	inline void reload() { release(), load(); }

	template <typename _Ty> requires LuaValidValue<_Ty> || std::same_as<_Ty, LuaRef>
	inline void setValue(std::string_view name, const _Ty& value) const { (*_env)[name] = value; }

	inline void setNil(std::string_view name) const { (*_env)[name] = nullptr; }

	template <typename _Ty> requires LuaValidValue<_Ty> || std::same_as<_Ty, LuaRef>
	inline _Ty getValue(std::string_view name) const
	{
		lua::bridge::TypeResult<_Ty> castResult = (*_env)[name].cast<_Ty>();
		if (!castResult)
		{
			logger::error("LuaModule Env error: {}", castResult.message());
			return _Ty();
		}
		return castResult.value();
	}

	template <>
	inline LuaRef getValue<LuaRef>(std::string_view name) const { return (*_env)[name]; }

	inline void setValueFromGlobal(std::string_view name) const { lua::utils::setFromGlobal(name, *_env); }

private:
	static void LUA_import(const std::string& path, LuaRef fromRoot);
	static void LUA_include(const std::string& path, LuaRef fromRoot);

public:
	static inline Reference<LuaModule> getCurrentRunningModule() { return LuaCallStack::top(); }

private:
	static LuaModule* makeAndLoad(const Path& path);

public:
	static inline std::shared_ptr<LuaModule> loadShared(const Path& path) { return std::shared_ptr<LuaModule>(makeAndLoad(path)); }
	static inline std::unique_ptr<LuaModule> loadUnique(const Path& path) { return std::unique_ptr<LuaModule>(makeAndLoad(path)); }
};



class LuaModuleImportManager
{
public:
	friend LuaModule;

private:
	static LuaModuleImportManager Instance;

private:
	std::unordered_map<Path, std::shared_ptr<LuaModule>> _modules;

public:
	LuaModuleImportManager(const LuaModuleImportManager&) = delete;
	LuaModuleImportManager(LuaModuleImportManager&&) noexcept = delete;

	LuaModuleImportManager& operator= (const LuaModuleImportManager&) = delete;
	LuaModuleImportManager& operator= (LuaModuleImportManager&&) noexcept = delete;

private:
	LuaModuleImportManager() = default;
	~LuaModuleImportManager() = default;

private:
	std::shared_ptr<LuaModule> load(const Path& path);

private:
	static LuaModuleImportManager& instance() { return Instance; }
};
