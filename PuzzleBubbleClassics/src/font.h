#pragma once

#include "utils/manager.h"
#include "utils/path.h"

#include <SFML/Graphics.hpp>


class FontManager : public Manager<sf::Font>
{
private:
	static FontManager Instance;

public:
	Pointer load(const Path& path);

	bool loadAllFromDirectory(const Path& path);

private:
	inline FontManager() : Manager(nullptr) {}

public:
	static constexpr FontManager& instance() { return Instance; }
};
