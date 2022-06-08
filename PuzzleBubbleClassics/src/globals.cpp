#include "globals.h"

#include "lua.h"
#include "fonts.h"
#include "assets.h"
#include "bubbles.h"


GlobalsManager GlobalsManager::instance;

GlobalsManager::GlobalsManager() :
	_luaGlobalState(new LuaGlobalState()),
	_scripts(new LuaScriptManager()),
	_fonts(new FontManager()),
	_textures(new TextureManager(0)),
	_bubblesManager(new BubbleModelManager())
{}

GlobalsManager::~GlobalsManager()
{
	delete _fonts;
	delete _textures;
	delete _bubblesManager;

	delete _scripts;
	delete _luaGlobalState;
}

namespace globals
{
	LuaGlobalState& luaGlobalState() { return *GlobalsManager::instance._luaGlobalState; }

	LuaScriptManager& luaScripts() { return *GlobalsManager::instance._scripts; }

	FontManager& fonts() { return *GlobalsManager::instance._fonts; }

	TextureManager& textures() { return *GlobalsManager::instance._textures; }

	BubbleModelManager& bubbleModels() { return *GlobalsManager::instance._bubblesManager; }
}
