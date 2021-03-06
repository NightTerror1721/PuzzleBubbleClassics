#pragma once

#include "common.h"
#include "globals.h"

#include "utils/SimpleElementManager.h"

class FontManager : public SimpleElementManager<sf::Font>
{
public:
	sf::Font& load(const String& filename, const String& name);

	void loadAll();

public:
	friend GlobalsManager;
};
