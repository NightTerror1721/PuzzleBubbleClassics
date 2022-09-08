#include "json.h"

namespace nl = nlohmann;

namespace utils::json
{
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
