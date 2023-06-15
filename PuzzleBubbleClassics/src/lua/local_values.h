#pragma once

#include "env.h"


class LuaLocalValuesContainer
{
private:
	mutable LuaRef _locals;

public:
	LuaLocalValuesContainer() noexcept = default;
	LuaLocalValuesContainer(const LuaLocalValuesContainer&) = default;
	LuaLocalValuesContainer(LuaLocalValuesContainer&&) noexcept = default;
	~LuaLocalValuesContainer() = default;

	LuaLocalValuesContainer& operator= (const LuaLocalValuesContainer&) = default;
	LuaLocalValuesContainer& operator= (LuaLocalValuesContainer&&) noexcept = default;

private:
	inline const LuaRef& locals() const
	{
		if (!_locals.isValid())
			_locals = lua::utils::newTableRef();
		return _locals;
	}

public:
	template <LuaValidValue _Ty>
	inline void setLocalValue(std::string_view name, const _Ty& value) { locals()[name] = value; }

	template <LuaValidValue _Ty>
	inline _Ty getLocalValue(std::string_view name) const
	{
		lua::bridge::TypeResult<_Ty> castResult = locals()[name].cast<_Ty>();
		if (!castResult)
		{
			logger::error("LocalValue error: {}", castResult.message());
			return _Ty();
		}
		return castResult.value();
	}

	template <>
	inline LuaRef getLocalValue(std::string_view name) const { return locals()[name]; }

	inline void deleteLocalValue(std::string_view name) { setLocalValue(name, nullptr); }

	inline void clearLocalValues() { lua::utils::resetRef(_locals); }
};


namespace lua::lib
{
	template <std::derived_from<LuaLocalValuesContainer> _Ty>
	inline bridge::Namespace::Class<_Ty>& addLocalValuesContainerToClass(bridge::Namespace::Class<_Ty>& cls)
	{
		const auto getFn = +[](const _Ty* self, const char* name) -> LuaRef { return self->getLocalValue<LuaRef>(name); };
		const auto setFn = +[](_Ty* self, const char* name, LuaRef value) -> void { self->setLocalValue(name, value); };
		const auto delFn = +[](_Ty* self, const char* name) -> void { self->deleteLocalValue(name); };

		return cls
			.addFunction("getLocalValue", getFn)
			.addFunction("setLocalValue", setFn)
			.addFunction("deleteLocalValue", delFn);
	}
}
