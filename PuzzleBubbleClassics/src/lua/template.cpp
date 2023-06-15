#include "template.h"


Reference<LuaRef> LuaTemplate::findLuaObject(std::string_view name) const
{
	if (!isLoaded())
		return nullptr;

	auto it = _luaCache.find(name.data());
	if (it != _luaCache.end())
		return std::addressof(it->second);

	LuaRef obj = _module->getValue<LuaRef>(name);
	if (!obj.isValid() || obj.isNil())
		return nullptr;

	_luaCache.insert({ name.data(), std::move(obj) });
	return std::addressof(_luaCache.at(name.data()));
}

LuaTemplate::~LuaTemplate()
{
	clear();
}

bool LuaTemplate::load(const std::string& name, const Path& path)
{
	if (isLoaded())
	{
		logger::warn("Attempt to load already loaded LuaModel {}.lua", _path.string());
		return false;
	}

	_name = name;
	_path = path;

	_module = LuaModule::loadUnique(_path);
	if (_module == nullptr || !_module->isLoaded())
	{
		logger::error("Cannot load LuaModel Script {} because not found or has errors on load.", _path.string());
		return false;
	}

	static constinit Id _idgen = 1;

	_id = _idgen++;
	_luaCache.clear();

	init();

	return true;
}

void LuaTemplate::reload()
{
	if (isLoaded())
	{
		clear();
		_luaCache.clear();
		_module->reload();
		init();
	}
}

void LuaTemplate::clear()
{
}

void LuaTemplate::init()
{
	vcall(FunctionOnInit);
}
