#include "resources.h"

namespace resource
{
	const Folder data = "data"_p;
	const Folder fonts(data, "fonts"_p);
	const Folder textures(data, "textures"_p);
	const Folder models(data, "models"_p);
	const Folder audio(data, "audio"_p);
	const Folder sounds(audio, "sounds"_p);
	const Folder music(audio, "music"_p);
}

namespace resource
{
	Folder::Folder(const Path& path) :
		_path{ path }
	{}
	Folder::Folder(const Folder& parent, const Path& path) :
		_path{ parent._path / path }
	{}

	std::strong_ordering Folder::operator<=> (const Folder& right) const
	{
		int cmp = _path.compare(right._path);
		if (cmp == 0)
			return std::strong_ordering::equal;
		else if (cmp > 0)
			return std::strong_ordering::greater;
		else
			return std::strong_ordering::less;
	}

	bool Folder::_open(const String& filename, std::ifstream& stream) const
	{
		stream.open(_path / filename, std::ios::in);
		return !stream.fail();
	}
	bool Folder::_open(const Path& path, std::ifstream& stream) const
	{
		stream.open(_path / path, std::ios::in);
		return !stream.fail();
	}
	bool Folder::_open(const String& filename, std::ofstream& stream) const
	{
		stream.open(_path / filename, std::ios::out);
		return !stream.fail();
	}
	bool Folder::_open(const Path& path, std::ofstream& stream) const
	{
		stream.open(_path / path, std::ios::out);
		return !stream.fail();
	}

	bool Folder::openInput(const String& filename, std::ifstream& input) const { return _open(filename, input); }
	bool Folder::openInput(const Path& path, std::ifstream& input) const { return _open(path, input); }
	bool Folder::openInput(const String& filename, const std::function<void(std::istream&)>& action) const
	{
		std::ifstream stream;
		if (_open(filename, stream))
			return action(stream), true;
		return false;
	}
	bool Folder::openInput(const Path& path, const std::function<void(std::istream&)>& action) const
	{
		std::ifstream stream;
		if (_open(path, stream))
			return action(stream), true;
		return false;
	}

	bool Folder::openOutput(const String& filename, std::ofstream& output) const { return _open(filename, output); }
	bool Folder::openOutput(const Path& path, std::ofstream& output) const { return _open(path, output); }
	bool Folder::openOutput(const String& filename, const std::function<void(std::ostream&)>& action) const
	{
		std::ofstream stream;
		if (_open(filename, stream))
			return action(stream), true;
		return false;
	}
	bool Folder::openOutput(const Path& path, const std::function<void(std::ostream&)>& action) const
	{
		std::ofstream stream;
		if (_open(path, stream))
			return action(stream), true;
		return false;
	}

	bool Folder::readJson(const String& filename, Json& json) const
	{
		return openInput(filename, [&json](std::istream& is) { json = utils::json::read(is); });
	}
	bool Folder::readJson(const Path& path, Json& json) const
	{
		return openInput(path, [&json](std::istream& is){ json = utils::json::read(is); });
	}

	bool Folder::writeJson(const String& filename, const Json& json) const
	{
		return openOutput(filename, [&json](std::ostream& os) { utils::json::write(os, json); });
	}
	bool Folder::writeJson(const Path& path, const Json& json) const
	{
		return openOutput(path, [&json](std::ostream& os) { utils::json::write(os, json); });
	}

	LuaScript Folder::getLuaScript(const Path& path) const
	{
		return lua::get_script(_path / path);
	}

	LuaScript Folder::runLuaScript(const Path& path) const
	{
		return lua::run_script(_path / path);
	}
}
