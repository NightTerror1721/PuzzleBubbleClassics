#include "font.h"

#include "utils/logger.h"
#include "utils/path.h"


FontManager FontManager::Instance;

FontManager::Pointer FontManager::load(const Path& path)
{
	std::string id = path.filename().replace_extension().string();
	if (contains(id))
		destroy(id);

	Pointer ptr = emplace(id);
	if (ptr == nullptr)
		return nullptr;

	if (!ptr->loadFromFile(path.string()))
	{
		logger::error("Font: Cannot open font file '{}'.", path.string());
		destroy(id);
		return nullptr;
	}

	return ptr;
}

bool FontManager::loadAllFromDirectory(const Path& path)
{
	if (!utils::path::isDirectory(path))
	{
		logger::error("Font Directory load: Expected valid directory, but found '{}'.", path.string());
		return false;
	}

	utils::path::scanDirectoryFiles(path, ".ttf", [this](const Path& filePath) {
		load(filePath);
	});

	return true;
}
