#pragma once

#include "utils/path.h"
#include "utils/json.h"
#include "utils/logger.h"

#include <compare>
#include <string>


namespace resources
{
	class Directory
	{
	private:
		Path _path;

	public:
		Directory(const Directory&) = default;
		Directory(Directory&&) noexcept = default;
		~Directory() = default;

		Directory& operator= (const Directory&) = default;
		Directory& operator= (Directory&&) noexcept = default;

		bool operator== (const Directory&) const = default;

	public:
		inline Directory(const Path& root) : _path(utils::path::normalize(root)) {}
		inline Directory(const Directory& parent, const Path& subPath) : _path(parent._path / subPath) {}

		inline const Path& path() const { return _path; }

		inline Path resolve(const Path& subPath) const { return _path / subPath; }
		inline Path operator/ (const Path& subPath) const { return _path / subPath; }

		inline std::ifstream openInputStream(const Path& subPath) const { return std::ifstream(resolve(subPath)); }
		inline std::ofstream openOutputStream(const Path& subPath) const { return std::ofstream(resolve(subPath)); }

		inline JsonValue readJson(const Path& subPath) const
		{
			auto is = openInputStream(subPath);
			if (!is)
			{
				logger::error("Cannot read json from '{}' file.", subPath.string());
				return {};
			}
			return json::read(is);
		}

		inline void writeJson(const Path& subPath, const JsonValue& json) const
		{
			auto os = openOutputStream(subPath);
			if (!os)
			{
				logger::error("Cannot write json to '{}' file.", subPath.string());
				return;
			}
			json::write(os, json);
		}
	};
}

namespace resources
{
	inline const Directory data = { "data" };
	inline const Directory mods = { data, "mods" };
}


