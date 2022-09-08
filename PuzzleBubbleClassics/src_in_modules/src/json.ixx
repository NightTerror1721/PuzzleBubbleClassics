module;

#include "headers.h"

export module JsonUtils;

import Common;


export
{
	using Json = nlohmann::basic_json<>;
	using JsonArray = nlohmann::json::array_t;
	using JsonObject = nlohmann::json::object_t;
}

export
template <typename _Ty>
concept SerializableToJson = requires(const _Ty & obj) { { obj.serialize() } -> std::same_as<Json>; };

export
template <typename _Ty>
concept DeserializableFromJson = requires(_Ty & obj, const Json & json) { { obj.deserialize(json) }; };

export
template <typename _Ty>
concept Jsonable = SerializableToJson<_Ty> && DeserializableFromJson<_Ty>;

export class JsonException : public std::exception
{
public:
	inline JsonException(const char* msg = "") : exception(msg) {}
	inline JsonException(const String& msg) : exception(msg.c_str()) {}
};

namespace utils::json
{
	export using value = nlohmann::json;

	export Json read(std::istream& is);
	export void write(std::ostream& os, const Json& json, unsigned int indent_step = 0);

	export Json read(const Path& path);
	export void write(const Path& path, const Json& json, unsigned int indent_step = 0);

	export Json parse(const char* str);
	export Json parse(const String& str);
}


// INLINE FUNCTIONS //
namespace utils::json
{
	export inline Json serialize(const SerializableToJson auto& object) { return object.serialize(); }
	export inline void deserialize(DeserializableFromJson auto& object, const Json& json) { object.deserialize(json); }
}


// TEMPLATE FUNCTIONS //
namespace utils::json
{
	export
	template <typename _Ty> requires DeserializableFromJson<_Ty>&& std::default_initializable<_Ty> && (std::movable<_Ty> || std::copyable<_Ty>)
	_Ty deserialize(const Json& json)
	{
		_Ty object = _Ty();
		object.deserialize(json);
		return object;
	}
}


// IMPLEMENTATION //
module :private;

namespace utils::json
{
	namespace nl = nlohmann;

	Json read(std::istream& is)
	{
		try
		{
			return Json::parse(is, nullptr, true, true);
		}
		catch (const std::exception& ex) { throw JsonException(ex.what()); }
	}

	Json read(const Path& path)
	{
		std::ifstream fs(path, std::ios::in);
		if (fs)
		{
			return read(fs);
		}
		return {};
	}

	Json parse(const char* str)
	{
		try
		{
			return Json::parse(String(str), nullptr, true, true);
		}
		catch (const std::exception& ex) { throw JsonException(ex.what()); }
	}

	Json parse(const String& str)
	{
		try
		{
			return Json::parse(str, nullptr, true, true);
		}
		catch (const std::exception& ex) { throw JsonException(ex.what()); }
	}

	void write(std::ostream& os, const Json& json, unsigned int indent_step)
	{
		try
		{
			nl::detail::serializer<Json> s(nl::detail::output_adapter<char>(os), ' ');
			s.dump(json, indent_step > 0, false, indent_step);
		}
		catch (const std::exception& ex) { throw JsonException(ex.what()); }
	}

	void write(const Path& path, const Json& json, unsigned int indent_step)
	{
		std::ofstream fs(path, std::ios::out);
		if (fs)
		{
			write(fs, json, indent_step);
		}
	}
}
