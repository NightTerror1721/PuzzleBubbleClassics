#pragma once

#include <compare>

#include "common.h"
#include "json.h"
#include "lua.h"

inline Path operator"" _p(const char* str, Size len)
{
	return Path(String(str, len));
}

namespace resource
{
	class Folder
	{
	private:
		Path _path;

	public:
		Folder() = default;
		Folder(const Folder&) = default;
		Folder(Folder&&) noexcept = default;
		~Folder() = default;

		Folder& operator= (const Folder&) = default;
		Folder& operator= (Folder&&) noexcept = default;

		bool operator== (const Folder&) const = default;

	public:
		Folder(const Path& path);
		Folder(const Folder& parent, const Path& path);

		std::strong_ordering operator<=> (const Folder& right) const;

		bool openInput(const String& filename, std::ifstream& input) const;
		bool openInput(const Path& path, std::ifstream& input) const;
		bool openInput(const String& filename, const std::function<void(std::istream&)>& action) const;
		bool openInput(const Path& path, const std::function<void(std::istream&)>& action) const;

		bool openOutput(const String& filename, std::ofstream& output) const;
		bool openOutput(const Path& path, std::ofstream& output) const;
		bool openOutput(const String& filename, const std::function<void(std::ostream&)>& action) const;
		bool openOutput(const Path& path, const std::function<void(std::ostream&)>& action) const;

		bool readJson(const String& filename, Json& json) const;
		bool readJson(const Path& path, Json& json) const;

		bool writeJson(const String& filename, const Json& json) const;
		bool writeJson(const Path& path, const Json& json) const;

		inline bool openInput(const char* filename, std::ifstream& input) const { return openInput(String{ filename }, input); }
		inline bool openInput(const char* filename, const std::function<void(std::istream&)>& action) const { return openInput(String{ filename }, action); }

		inline bool openOutput(const char* filename, std::ofstream& output) const { return openOutput(String{ filename }, output); }
		inline bool openOutput(const char* filename, const std::function<void(std::ostream&)>& action) const { return openOutput(String{ filename }, action); }

		inline bool readJson(const char* filename, Json& json) const { return readJson(String{ filename }, json); }

		inline bool writeJson(const char* filename, const Json& json) const { return writeJson(String{ filename }, json); }

		template <DeserializableFromJson _Ty>
		inline _Ty& readAndInject(const String& filename, _Ty& obj) const
		{
			return openInput(filename, [&obj](std::istream& in) { utils::json::deserialize(obj, utils::json::read(in)); }), obj;
		}

		template <DeserializableFromJson _Ty>
		inline _Ty& readAndInject(const Path& path, _Ty& obj) const
		{
			return openInput(path, [&obj](std::istream& in) { utils::json::deserialize(obj, utils::json::read(in)); }), obj;
		}

		template <DeserializableFromJson _Ty>
		inline _Ty& readAndInject(const char* filename, _Ty& obj) const
		{
			return readAndInject(String{ filename }, obj);
		}

		template <SerializableToJson _Ty>
		inline void extractAndWrite(const String& filename, SerializableToJson auto& obj) const
		{
			openOutput(filename, [&obj](std::ostream& os) { utils::json::write(os, utils::json::serialize(obj)); });
		}

		template <SerializableToJson _Ty>
		inline void extractAndWrite(const Path& path, _Ty& obj) const
		{
			openOutput(path, [&obj](std::ostream& os) { utils::json::write(os, utils::json::serialize(obj)); });
		}

		template <SerializableToJson _Ty>
		inline void extractAndWrite(const char* filename, _Ty& obj) const
		{
			extractAndWrite(String{ filename }, obj);
		}

		LuaScript getLuaScript(const Path& path) const;
		LuaScript runLuaScript(const Path& path) const;

		inline LuaScript getLuaScript(const String& filename) const { return getLuaScript(Path(filename)); }
		inline LuaScript getLuaScript(const char* filename) const { return getLuaScript(Path(filename)); }

		inline LuaScript runLuaScript(const String& filename) const { return runLuaScript(Path(filename)); }
		inline LuaScript runLuaScript(const char* filename) const { return runLuaScript(Path(filename)); }


	public:
		inline Path pathOf(const String& filename) const { return _path / filename; }
		inline Path pathOf(const Path& path) const { return _path / path; }
		inline Path pathOf(const char* filename) const { return _path / filename; }

		inline Folder folder(const String& filename) const { return { *this, filename }; }
		inline Folder folder(const Path& path) const { return { *this, path }; }
		inline Folder folder(const char* path) const { return { *this, String{ path } }; }

		inline const Path& path() const { return _path; }

		inline Path operator/ (const String& filename) const { return pathOf(filename); }
		inline Path operator/ (const Path& path) const { return pathOf(path); }
		inline Path operator/ (const char* filename) const { return pathOf(filename); }

	private:
		bool _open(const String& filename, std::ifstream& stream) const;
		bool _open(const Path& path, std::ifstream& stream) const;

		bool _open(const String& filename, std::ofstream& stream) const;
		bool _open(const Path& path, std::ofstream& stream) const;
	};
}

namespace resource
{
	extern const Folder data;
	extern const Folder fonts;
	extern const Folder textures;
	extern const Folder models;
	extern const Folder audio;
	extern const Folder sounds;
	extern const Folder music;
}
