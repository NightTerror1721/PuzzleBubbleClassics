#pragma once

#include "module.h"

#include "utils/manager.h"
#include "utils/path.h"


class LuaTemplate
{
protected:
	static constexpr std::string_view FunctionOnInit = "OnInit";

public:
	using Id = unsigned int;

protected:
	Id _id = 0;
	std::string _name = {};
	Path _path = {};
	std::unique_ptr<LuaModule> _module = nullptr;

private:
	mutable std::unordered_map<std::string, LuaRef> _luaCache;

public:
	LuaTemplate() = default;
	LuaTemplate(const LuaTemplate&) = delete;
	LuaTemplate(LuaTemplate&&) noexcept = default;

	LuaTemplate& operator= (const LuaTemplate&) = delete;
	LuaTemplate& operator= (LuaTemplate&&) noexcept = default;

	constexpr bool operator== (const LuaTemplate& right) const noexcept { return _id == right._id; }
	constexpr std::strong_ordering operator<=> (const LuaTemplate& right) const noexcept { return _id <=> right._id; }

public:
	constexpr Id getId() const { return _id; }
	constexpr const std::string& getName() const { return _name; }
	constexpr const Path& getPath() const { return _path; }

	inline bool isLoaded() const { return _module != nullptr; }

public:
	virtual ~LuaTemplate();

	virtual bool load(const std::string& name, const Path& path);

	void reload();

protected:
	virtual void clear();

private:
	void init();

protected:
	Reference<LuaRef> findLuaObject(std::string_view name) const;

	template <typename... _ArgsTys>
	inline void vcall(std::string_view name, _ArgsTys&&... args)
	{
		auto fn = findLuaObject(name);
		if (fn != nullptr)
		{
			try
			{
				lua::bridge::LuaResult result = (*fn)(std::forward<_ArgsTys>(args)...);
				if (result.hasFailed())
					logger::error("Lua function {} call error: {}", name, result.errorMessage());
			}
			catch (const LuaException& ex)
			{
				logger::error("Lua function {} call error: {}", name, ex.what());
			}
		}
	}

	template <typename _RetTy, typename... _ArgsTys>
	inline _RetTy call(std::string_view name, _ArgsTys&&... args)
	{
		auto fn = findLuaObject(name);
		if (fn != nullptr)
		{
			try
			{
				if constexpr (std::same_as<LuaRef, _RetTy>)
				{
					lua::bridge::LuaResult result = (*fn)(std::forward<_ArgsTys>(args)...);
					if (result.hasFailed())
					{
						logger::error("Lua function {} call error: {}", name, result.errorMessage());
						return _RetTy();
					}
					else
						return result[0];
				}
				else
				{
					lua::bridge::LuaResult result = (*fn)(std::forward<_ArgsTys>(args)...);
					if (result.hasFailed())
					{
						logger::error("Lua function {} call error: {}", name, result.errorMessage());
						return _RetTy();
					}
					else
						return result[0].unsafe_cast<_RetTy>();
				}
			}
			catch (const LuaException& ex)
			{
				logger::error("Lua function {} call error: {}", name, ex.what());
				if constexpr (std::same_as<LuaRef, _RetTy>)
					return LuaRef(lua::state());
				else
					return LuaRef(lua::state()).cast<_RetTy>().value();
			}
		}
		else
		{
			if constexpr (std::same_as<LuaRef, _RetTy>)
				return LuaRef(lua::state());
			else
				return LuaRef(lua::state()).cast<_RetTy>().value();
		}
	}
};



template <std::derived_from<LuaTemplate> _LuaModelTy>
class LuaTemplateManager : public Manager<_LuaModelTy>
{
private:
	using ManagerType = Manager<_LuaModelTy>;

protected:
	inline LuaTemplateManager() : ManagerType(nullptr) {}

protected:
	std::shared_ptr<_LuaModelTy> loadTemplate(const Path& path)
	{
		Path normalizedPath = utils::path::normalize(path);
		std::string name = utils::path::getFileName(normalizedPath, false);
		std::shared_ptr<_LuaModelTy> ref = this->get(name);
		if (ref != nullptr)
			this->destroy(name);

		ref = this->emplace<_LuaModelTy>(name);
		if (ref == nullptr)
		{
			logger::error("Cannot load '{}' Model!", normalizedPath.string());
			return nullptr;
		}

		if (!ref->load(name, normalizedPath))
		{
			this->destroy(name);
			return nullptr;
		}

		return ref;
	}
};
