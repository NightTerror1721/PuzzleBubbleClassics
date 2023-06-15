#include "native_module.h"


LuaNativeModuleManager LuaNativeModuleManager::Instance;

std::shared_ptr<LuaNativeModule> LuaNativeModuleManager::get(const std::string& name) const
{
	const auto it = _modules.find(name);
	if (it == _modules.end())
		return nullptr;
	return it->second;
}

bool LuaNativeModuleManager::registerModule(const std::string& name, const LuaNativeModuleConstructor& constructor, std::initializer_list<std::string> dependences)
{
	if (_modules.contains(name))
	{
		logger::error("Lua PuzzleBubbleClassics native module {} already registered.", name);
		return false;
	}

	_modules.insert({ name, std::make_shared<LuaNativeModule>(name, constructor, dependences) });
	return true;
}


bool LuaNativeModule::open(const LuaRef& env, const std::function<bool(const LuaNativeModule&)>& condition, bool importMode) const
{
	if (!condition(*this))
		return true;

	if (hasDependences())
	{
		for (const auto& dep : _dependences)
		{
			auto mod = LuaNativeModuleManager::instance().get(dep);
			if (mod == nullptr)
			{
				lua::utils::error("Internal error: Native Module {} not found.", dep);
				return false;
			}
			mod->open(env, condition, importMode);
		}
	}

	if (!_built)
	{
		auto globalNamespace = lua::utils::getGlobalNamespace();
		auto libraryNamespace = globalNamespace.beginNamespace(_name.c_str());

		bool result = _constructor(libraryNamespace, lua::state());
		libraryNamespace.endNamespace();

		if (!result)
		{
			lua::utils::error("Internal error: Error during native module opening: {}", _name);
			return false;
		}

		_built = true;
	}

	LuaRef libRef = lua::utils::getGlobalValue(_name.c_str());
	if (!libRef.isTable())
	{
		lua::utils::error("Internal error: Unknown error on {} native module.", _name);
		return false;
	}

	if (importMode)
	{
		LuaRef proxyRef = lua::utils::newTableRef();
		lua::utils::forEachInTable(libRef, [&env, &proxyRef](const LuaRef& key, const LuaRef& value) {
			proxyRef[key] = value;
		});
		env[_name] = proxyRef;
	}
	else
	{
		lua::utils::forEachInTable(libRef, [&env](const LuaRef& key, const LuaRef& value) {
			env[key] = value;
		});
	}

	return true;
}
