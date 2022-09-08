#pragma once

#include "common.h"

#include <nlohmann/json.hpp>

typedef nlohmann::json Json;
typedef nlohmann::json::array_t JsonArray;
typedef nlohmann::json::object_t JsonObject;

template <typename _Ty>
concept SerializableToJson = requires(const _Ty & obj) { { obj.serialize() } -> std::same_as<Json>; };

template <typename _Ty>
concept DeserializableFromJson = requires(_Ty & obj, const Json & json) { { obj.deserialize(json) }; };

template <typename _Ty>
concept Jsonable = SerializableToJson<_Ty> && DeserializableFromJson<_Ty>;

class JsonException : public std::exception
{
public:
	inline JsonException(const char* msg = "") : exception(msg) {}
	inline JsonException(const String& msg) : exception(msg.c_str()) {}
};

namespace utils::json
{
	Json read(std::istream& is);
	void write(std::ostream& os, const Json& json, unsigned int indent_step = 0);

	Json read(const Path& path);
	void write(const Path& path, const Json& json, unsigned int indent_step = 0);

	Json parse(const char* str);
	Json parse(const String& str);

	inline Json serialize(const SerializableToJson auto& object) { return object.serialize(); }
	inline void deserialize(DeserializableFromJson auto& object, const Json& json) { object.deserialize(json); }

	template <typename _Ty> requires DeserializableFromJson<_Ty> && std::default_initializable<_Ty> && (std::movable<_Ty> || std::copyable<_Ty>)
	_Ty deserialize(const Json& json)
	{
		_Ty object();
		object.deserialize(json);
		return object;
	}
}
