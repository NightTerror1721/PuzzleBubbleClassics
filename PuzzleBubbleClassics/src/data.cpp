#include "data.h"

#include "font.h"


void ResourcePackage::forEachValidDirectory(const std::function<void(Reference<resources::Directory>, ResourceDirectoryType)>& action)
{
	for (ResourceDirectoryType type = ResourceDirectoryType::First; type < ResourceDirectoryType::Count; type = ResourceDirectoryType(int(type) + 1))
		if (_dirs[int(type)] != nullptr)
			action(_dirs[int(type)].get(), type);
}



static constexpr std::string_view resourceDirectoryName(ResourceDirectoryType type)
{
	using enum ResourceDirectoryType;
	switch (type)
	{
		case Bubbles: return "bubbles";
		case Textures: return "textures";
		case Fonts: return "fonts";
		case Audio: return "audio";
		default: return "invalid-resource-directory";
	}
}


bool ResourcePackage::load()
{
	const JsonValue config = loadConfigFile();

	if (config.contains(DirectoriesConfigField))
	{
		const JsonValue& directories = config.at(DirectoriesConfigField);
		if (directories.is_object())
		{
			for (ResourceDirectoryType type = ResourceDirectoryType::First; type < ResourceDirectoryType::Count; type = ResourceDirectoryType(int(type) + 1))
			{
				const std::string_view name = resourceDirectoryName(type);
				if (directories.contains(name))
				{
					const JsonValue& jDirName = directories.at(name);
					if (jDirName.is_string())
					{
						const std::string& dirName = jDirName.get_ref<const std::string&>();
						const Path dirPath = utils::path::normalize(_root.resolve(dirName));
						if (utils::path::isDirectory(dirPath))
						{
							_dirs[std::size_t(type)].reset(new resources::Directory(dirPath));
						}
					}
				}
			}
		}
	}

	return true;
}

bool ResourcePackage::loadDefault()
{
	for (ResourceDirectoryType type = ResourceDirectoryType::First; type < ResourceDirectoryType::Count; type = ResourceDirectoryType(int(type) + 1))
	{
		const std::string_view name = resourceDirectoryName(type);
		_dirs[std::size_t(type)].reset(new resources::Directory(_root, name));
	}

	return true;
}

JsonValue ResourcePackage::loadConfigFile()
{
	JsonValue json = _root.readJson(PackageConfigFileName);
	if (!json.is_object())
		return JsonObject();
	return json;
}




DataPool DataPool::Instance;

bool DataPool::load()
{
	_packages.clear();

	if (!loadDefaultPackage())
		return false;

	if (!loadPackages())
		return false;

	buildPaths();

	return true;
}

bool DataPool::loadDefaultPackage()
{
	std::unique_ptr<ResourcePackage> package = std::unique_ptr<ResourcePackage>(new ResourcePackage(resources::data.path()));
	if (!package->loadDefault())
	{
		logger::error("Default Package: Error during loading resources.");
		return false;
	}
	Reference<ResourcePackage> ref = package.get();
	_packages.push_back(std::move(package));
	return loadPackageData(ref);
}

bool DataPool::loadPackages()
{
	bool result = true;
	utils::path::scanDirectory(resources::mods.path(), [this, &result](const Path& elemPath) {
		if (utils::path::isDirectory(elemPath))
		{
			if (!loadPackage(elemPath))
				result = false;
		}
	});
	return result;
}

bool DataPool::loadPackage(const Path& path)
{
	std::unique_ptr<ResourcePackage> package = std::unique_ptr<ResourcePackage>(new ResourcePackage(path));
	if (!package->load())
	{
		logger::error("Package '{}': Error during loading resources.", path.string());
		return false;
	}
	Reference<ResourcePackage> ref = package.get();
	_packages.push_back(std::move(package));
	return loadPackageData(ref);
}

bool DataPool::loadPackageData(Reference<ResourcePackage> package)
{
	if (package->hasDirectory(ResourceDirectoryType::Fonts))
		FontManager::instance().loadAllFromDirectory(package->getDirectory(ResourceDirectoryType::Fonts)->path());

	return true;
}

void DataPool::buildPaths()
{
	for (auto& v : _paths)
		v.clear();

	const auto end = _packages.crend();
	for (auto it = _packages.crbegin(); it != end; it++)
	{
		(*it)->forEachValidDirectory([this](Reference<resources::Directory> dir, ResourceDirectoryType type) {
			_paths[int(type)].push_back(dir);
		});
	}
}


std::optional<Path> DataPool::findFilePath(ResourceDirectoryType type, std::string_view name, std::initializer_list<std::string_view> extensions)
{
	for (const auto& dir : _paths[int(type)])
	{
		auto filePath = utils::path::findFirstValidPath(dir->path(), name, extensions);
		if (filePath.has_value())
			return filePath;
	}
	return {};
}

void DataPool::forEachDirectory(ResourceDirectoryType type, const std::function<void(Reference<resources::Directory>)>& action)
{
	for (const auto& dir : _paths[int(type)])
		action(dir);
}


static void scanDirectoryPaths(const Path& dir, const std::function<void(const Path&)>& action, bool deep)
{
	utils::path::scanDirectory(dir, [&action, deep](const Path& path) {
		if (utils::path::isDirectory(path))
		{
			if (deep)
				scanDirectoryPaths(path, action, deep);
		}
		else if (utils::path::isFile(path))
			action(path);
	});
}
void DataPool::forEachDirectoryPath(ResourceDirectoryType type, const std::function<void(const Path&)>& action, bool deep)
{
	for (const auto& dir : _paths[int(type)])
		scanDirectoryPaths(dir->path(), action, deep);
}
