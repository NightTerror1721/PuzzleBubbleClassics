#include "fonts.h"
#include "resources.h"


sf::Font& FontManager::load(const String& filename, const String& name)
{
	sf::Font* f = create(name);

	if (!f)
		throw std::exception{ "An error has been ocurred during font creation." };

	if (!f->loadFromFile(resource::fonts.pathOf(filename).string()))
		throw std::exception{ "An error has been ocurred during font load." };

	return *f;
}

void FontManager::loadAll()
{
	Json json;
	resource::fonts.readJson("config.json", json);

	if (!json.is_object())
		return;

	for (auto it = json.begin(); it != json.end(); it++)
	{
		const Json& value = *it;
		if (!value.is_string())
			continue;

		try { load(value, it.key()); }
		catch (const std::exception& ex) { std::cerr << ex.what() << std::endl; }
	}
}