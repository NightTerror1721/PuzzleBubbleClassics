#pragma once

#include "utils/path.h"
#include "utils/json.h"
#include "utils/reference.h"

#include "resources.h"

#include <array>
#include <memory>
#include <vector>


enum class ResourceDirectoryType
{
	First = 0,
	
	Bubbles = First,
	Textures,
	Fonts,
	Audio,

	Count,
	Last = Count - 1
};


class DataPool;

class ResourcePackage
{
public:
	using DirectoriesArray = std::array<std::unique_ptr<resources::Directory>, std::size_t(ResourceDirectoryType::Count)>;

	friend DataPool;

private:
	static constexpr std::string_view PackageConfigFileName = "package.json";
	static constexpr std::string_view DirectoriesConfigField = "directories";


private:
	resources::Directory _root;
	DirectoriesArray _dirs;

public:
	ResourcePackage() = delete;
	ResourcePackage(const ResourcePackage&) = delete;
	ResourcePackage(ResourcePackage&&) noexcept = delete;
	~ResourcePackage() = default;

	ResourcePackage& operator= (const ResourcePackage&) = delete;
	ResourcePackage& operator= (ResourcePackage&&) noexcept = delete;

private:
	ResourcePackage(const Path& path) : _root(path), _dirs() {}

	bool load();
	bool loadDefault();
	JsonValue loadConfigFile();

public:
	constexpr Reference<resources::Directory> getDirectory(ResourceDirectoryType type) const
	{
		if (type < ResourceDirectoryType::First || type > ResourceDirectoryType::Last)
			return nullptr;

		auto& ptr = _dirs[std::size_t(type)];
		if (ptr == nullptr)
			return nullptr;
		return ptr.get();
	}

	constexpr bool hasDirectory(ResourceDirectoryType type) const { return getDirectory(type); }

	constexpr Reference<resources::Directory> operator[] (ResourceDirectoryType type) { return getDirectory(type); }

public:
	void forEachValidDirectory(const std::function<void(Reference<resources::Directory>, ResourceDirectoryType)>& action);
};



class DataPool
{
private:
	using PathsArray = std::array<std::vector<Reference<resources::Directory>>, std::size_t(ResourceDirectoryType::Count)>;

private:
	static DataPool Instance;

private:
	std::vector<std::unique_ptr<ResourcePackage>> _packages;
	PathsArray _paths;

public:
	DataPool(const DataPool&) = delete;
	DataPool(DataPool&&) noexcept = delete;

	DataPool& operator= (const DataPool&) = delete;
	DataPool& operator= (DataPool&&) noexcept = delete;

private:
	DataPool() = default;
	~DataPool() = default;

public:
	bool load();

	std::optional<Path> findFilePath(ResourceDirectoryType type, std::string_view name, std::initializer_list<std::string_view> extensions);

	void forEachDirectory(ResourceDirectoryType type, const std::function<void(Reference<resources::Directory>)>& action);

	void forEachDirectoryPath(ResourceDirectoryType type, const std::function<void(const Path&)>& action, bool deep = true);

public:
	inline std::optional<Path> findFilePath(ResourceDirectoryType type, std::string_view name, std::string_view extension) { return findFilePath(type, name, { extension }); }
	inline std::optional<Path> findFilePath(ResourceDirectoryType type, std::string_view name) { return findFilePath(type, name, {}); }

private:
	bool loadDefaultPackage();
	bool loadPackages();
	bool loadPackage(const Path& path);
	bool loadPackageData(Reference<ResourcePackage> package);

	void buildPaths();

public:
	static constexpr DataPool& instance() { return Instance; }
};
