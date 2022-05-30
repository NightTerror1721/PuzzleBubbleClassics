#pragma once

#include "common.h"
#include "json.h"



namespace gprops
{
	void load();
	void store();

	const std::optional<Json> get_property(const String& name);
	void set_property(const String& name, const Json& value);
	bool delete_property(const String& name);

	void dump(std::ostream& os = std::cout, unsigned int indent_step = 0);


	inline void set(const String& name, std::integral auto value) { set_property(name, value); }
	inline void set(const String& name, std::floating_point auto value) { set_property(name, value); }
	inline void set(const String& name, bool value) { set_property(name, value); }
	inline void set(const String& name, const char* value) { set_property(name, value); }
	inline void set(const String& name, const String& value) { set_property(name, value); }
	inline void set(const String& name, const JsonArray& value) { set_property(name, value); }
	inline void set(const String& name, const JsonObject& value) { set_property(name, value); }


	template <typename _Ty> requires
		std::integral<_Ty> ||
		std::floating_point<_Ty> ||
		std::same_as<_Ty, bool>
	inline _Ty get(const String & name, _Ty default_value = 0)
	{
		auto value = get_property(name);
		return value.has_value() ? value->get<_Ty>() : default_value;
	}

	inline const char* get(const String& name, const char* default_value = nullptr)
	{
		auto value = get_property(name);
		return value.has_value() ? value->get<String>().c_str() : default_value;
	}

	template <typename _Ty> requires
		std::same_as<_Ty, String> ||
		std::same_as<_Ty, JsonArray> ||
		std::same_as<_Ty, JsonObject>
	inline const _Ty& get(const String& name, const _Ty& default_value = _Ty())
	{
		auto value = get_property(name);
		return value.has_value() ? value->get<_Ty>() : default_value;
	}
}
