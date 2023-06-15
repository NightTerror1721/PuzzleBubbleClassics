#pragma once

#include <functional>
#include <filesystem>
#include <optional>


using Path = std::filesystem::path;

namespace utils::path
{
	std::optional<Path> findFirstValidPath(const Path& root, std::string_view name, std::initializer_list<std::string_view> extensions);

	void scanDirectory(const Path& directory, const std::function<void(const Path&)>& action);

	void scanDirectoryFiles(const Path& directory, std::initializer_list<std::string_view> extensions, const std::function<void(const Path&)>& action);
}

namespace utils::path
{
	inline Path absolute(const Path& path) { return std::filesystem::absolute(path); }

	inline Path normalize(const Path& path) { return std::filesystem::absolute(std::filesystem::weakly_canonical(path)); }

	inline std::optional<Path> findFirstValidPath(const Path& root, std::string_view name, std::string_view extension)
	{
		return findFirstValidPath(root, name, { extension });
	}

	inline std::optional<Path> findFirstValidPath(const Path& root, std::string_view name)
	{
		return findFirstValidPath(root, name, {});
	}

	inline void scanDirectoryFiles(const Path& directory, std::string_view extension, const std::function<void(const Path&)>& action)
	{
		scanDirectoryFiles(directory, { extension }, action);
	}

	inline bool exists(const Path& path) { return std::filesystem::exists(path); }

	inline bool isFile(const Path& path) { return std::filesystem::is_regular_file(path); }

	inline bool isDirectory(const Path& path) { return std::filesystem::is_directory(path); }

	inline std::string getFileName(const Path& path, bool withExtension = true)
	{
		Path filename = path.filename();
		if (withExtension)
			return filename.string();
		return filename.replace_extension().string();
	}

	inline bool hasExtension(const Path& path, std::string_view extension)
	{
		return path.has_extension() && path.extension().string() == extension;
	}
}
