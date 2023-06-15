#include "module.h"

#include "constants.h"
#include "native_module.h"
#include "resources.h"


LuaCallStack LuaCallStack::Instance;


LuaModule::LuaModule(const Path& path, Reference<LuaModule> parent) :
	_parent(parent),
	_env(),
	_path(utils::path::normalize(path)),
	_directory(Path(_path).remove_filename()),
	_importedNativeModules(),
	_includedNativeModules(),
	_importedModules(),
	_includedModules(),
	_chunk(lua::utils::newRef()),
	_loaded(false)
{}

bool LuaModule::load()
{
	if (isLoaded())
		return false;

	if (_parent == nullptr)
	{
		_env = std::make_shared<LuaRef>(lua::utils::newTableRef());
		loadBuiltinElements();
	}
	else _env = _parent->_env;

	_importedNativeModules = _parent == nullptr ? std::make_shared<std::unordered_set<std::string>>() : _parent->_importedNativeModules;
	_includedNativeModules = _parent == nullptr ? std::make_shared<std::unordered_set<std::string>>() : _parent->_includedNativeModules;
	_importedModules = _parent == nullptr ? std::make_shared<std::unordered_set<Path>>() : _parent->_importedModules;
	_includedModules = _parent == nullptr ? std::make_shared<std::unordered_set<Path>>() : _parent->_includedModules;

	lua_State* const state = lua::state();
	int status = luaL_loadfile(state, _path.string().c_str());
	if (status != LUA_OK)
	{
		release();
		if (_parent != nullptr)
			lua::utils::error(lua::utils::extractErrorFromStack());
		else
			logger::error(lua::utils::extractErrorFromStack());
		return false;
	}

	_chunk = lua::utils::refFromStack();
	_loaded = true;

	return run();
}

bool LuaModule::run()
{
	lua_State* const state = lua::state();
	bool hasError = false;
	std::string errorMsg;

	_chunk.push();
	_env->push();
	lua_setupvalue(state, -2, 1);

	LuaCallStack::push(*this);
	try { lua::bridge::pcall(state); }
	catch (const LuaException& ex)
	{
		hasError = true;
		errorMsg = ex.what();
	}
	LuaCallStack::pop();

	if (hasError)
	{
		release();
		if (_parent != nullptr)
			lua::utils::error(errorMsg);
		else
			logger::error(errorMsg);
		return false;
	}

	_includedModules->insert(_path);

	return true;
}

void LuaModule::release()
{
	_loaded = false;
	lua::utils::resetRef(_chunk);
	_importedNativeModules.reset();
	_includedNativeModules.reset();
	_includedModules.reset();
	_env.reset();
}

Path LuaModule::findRelativePath(const std::string_view& spath)
{
	Path path{ spath };
	if (!path.is_absolute())
		path = utils::path::normalize(_directory / path);

	if (!path.has_extension())
		path += ".lua";

	return utils::path::normalize(path);
}

Path LuaModule::findPathFromRoot(const std::string_view& spath)
{
	Path path{ spath };
	if (!path.is_absolute())
		path = utils::path::normalize(resources::data / path);

	if (!path.has_extension())
		path += ".lua";

	return utils::path::normalize(path);
}


bool LuaModule::importModule(const std::string& spath, bool fromRoot)
{
	if (!isLoaded())
	{
		lua::utils::error("Cannot import other modules on unloaded module!");
		return false;
	}

	const auto natMod = LuaNativeModuleManager::instance().get(spath);
	if (natMod != nullptr && !fromRoot)
	{
		const auto& imported = _importedNativeModules;
		return natMod->importIn(*_env, [&imported](const LuaNativeModule& nativeModule) -> bool {
			return !imported->contains(nativeModule.getName());
		});
	}

	Path path = fromRoot ? findPathFromRoot(spath) : findRelativePath(spath);
	if (_importedModules->contains(path))
		return true;

	const auto impMod = LuaModuleImportManager::instance().load(path);
	if (impMod == nullptr)
		return false;

	_importedModules->insert(std::move(path));

	LuaRef customName = impMod->getValue<LuaRef>("__name");
	if (!customName.isString())
	{
		lua::utils::error("Invalid Lua Module '__name' field on module '{}'.", path.string());
		return false;
	}

	std::string name = customName.unsafe_cast<std::string>();

	LuaRef table = lua::utils::newTableRef();
	lua::utils::copyTableEntries(*impMod->_env, table);
	(*_env)[name] = table;
	return true;
}

bool LuaModule::includeModule(const std::string& spath, bool fromRoot)
{
	if (!isLoaded())
	{
		lua::utils::error("Cannot include sub-modules on unloaded module!");
		return false;
	}

	const auto natMod = LuaNativeModuleManager::instance().get(spath);
	if (natMod != nullptr && !fromRoot)
	{
		const auto& included = _includedNativeModules;
		return natMod->includeIn(*_env, [&included](const LuaNativeModule& nativeModule) -> bool {
			return !included->contains(nativeModule.getName());
		});
	}

	Path path = fromRoot ? findPathFromRoot(spath) : findRelativePath(spath);
	if (_includedModules->contains(path))
		return true;

	if (!LuaModule(path, this).load())
	{
		lua::utils::error("Error during loading of {} module!", path.string());
		return false;
	}

	return true;
}

void LuaModule::LUA_import(const std::string& path, LuaRef luaFromRoot)
{
	auto mod = getCurrentRunningModule();
	if (mod == nullptr)
	{
		lua::utils::error("'import' function can only use on global scope.");
		return;
	}

	bool fromRoot = luaFromRoot.unsafe_cast<bool>();
	mod->importModule(path, fromRoot);
}

void LuaModule::LUA_include(const std::string& path, LuaRef luaFromRoot)
{
	auto mod = getCurrentRunningModule();
	if (mod == nullptr)
	{
		lua::utils::error("'include' function can only use on global scope.");
		return;
	}

	bool fromRoot = luaFromRoot.unsafe_cast<bool>();
	mod->includeModule(path, fromRoot);
}

void LuaModule::loadBuiltinElements()
{
	static constinit bool initiated = false;
	if (!initiated)
	{
		lua::utils::getGlobalNamespace()
			.addFunction(lua::constants::import, *LUA_import)
			.addFunction(lua::constants::include, *LUA_include);
		initiated = true;
	}

	setValueFromGlobal(lua::constants::import);
	setValueFromGlobal(lua::constants::include);

	setValueFromGlobal(lua::constants::xpcall);
	setValueFromGlobal(lua::constants::select);
	setValueFromGlobal(lua::constants::utf8);
	setValueFromGlobal(lua::constants::next);
	setValueFromGlobal(lua::constants::base);
	setValueFromGlobal(lua::constants::load);
	setValueFromGlobal(lua::constants::getmetatable);
	setValueFromGlobal(lua::constants::tonumber);
	setValueFromGlobal(lua::constants::string);
	setValueFromGlobal(lua::constants::math);
	setValueFromGlobal(lua::constants::_VERSION);
	setValueFromGlobal(lua::constants::rawget);
	setValueFromGlobal(lua::constants::rawset);
	setValueFromGlobal(lua::constants::rawequal);
	setValueFromGlobal(lua::constants::rawlen);
	setValueFromGlobal(lua::constants::error);
	setValueFromGlobal(lua::constants::tostring);
	setValueFromGlobal(lua::constants::table);
	setValueFromGlobal(lua::constants::collectgarbage);
	setValueFromGlobal(lua::constants::warn);
	setValueFromGlobal(lua::constants::pcall);
	setValueFromGlobal(lua::constants::loadfile);
	setValueFromGlobal(lua::constants::ipairs);
	setValueFromGlobal(lua::constants::setmetatable);
	setValueFromGlobal(lua::constants::coroutine);
	setValueFromGlobal(lua::constants::type);
	setValueFromGlobal(lua::constants::assert);
	setValueFromGlobal(lua::constants::dofile);
	setValueFromGlobal(lua::constants::os);
	setValueFromGlobal(lua::constants::print);
	setValueFromGlobal(lua::constants::pairs);
}

LuaModule* LuaModule::makeAndLoad(const Path& path)
{
	LuaModule* mod = new LuaModule(path, nullptr);
	if (!mod->load())
	{
		delete mod;
		return nullptr;
	}
	return mod;
}





LuaModuleImportManager LuaModuleImportManager::Instance;

std::shared_ptr<LuaModule> LuaModuleImportManager::load(const Path& rawPath)
{
	Path path = utils::path::normalize(rawPath);
	const auto it = _modules.find(path);
	if (it != _modules.end())
		return it->second;

	std::shared_ptr<LuaModule> mod{ new LuaModule(path, nullptr) };
	if (!mod->load())
		return nullptr;

	_modules.insert({ std::move(path), mod });

	LuaRef moduleName = mod->getValue<LuaRef>("__name");
	if (!moduleName.isString())
		mod->setValue("__name", path.filename().replace_extension().string());

	return mod;
}
