#pragma once


class LuaGlobalState;
class FontManager;
class TextureManager;
class BubbleModelManager;
class LuaScriptManager;


namespace globals
{
	LuaGlobalState& luaGlobalState();
	LuaScriptManager& luaScripts();
	FontManager& fonts();
	TextureManager& textures();
	BubbleModelManager& bubbleModels();
}


class GlobalsManager
{
private:
	LuaGlobalState* _luaGlobalState;
	LuaScriptManager* _scripts;
	FontManager* _fonts;
	TextureManager* _textures;
	BubbleModelManager* _bubblesManager;

private:
	static GlobalsManager instance;

private:
	GlobalsManager();

public:
	GlobalsManager(const GlobalsManager&) = delete;
	GlobalsManager(GlobalsManager&&) noexcept = delete;

	GlobalsManager& operator= (const GlobalsManager&) = delete;
	GlobalsManager& operator= (GlobalsManager&&) noexcept = delete;

public:
	~GlobalsManager();

public:
	friend LuaGlobalState& ::globals::luaGlobalState();
	friend LuaScriptManager& ::globals::luaScripts();
	friend FontManager& ::globals::fonts();
	friend TextureManager& ::globals::textures();
	friend BubbleModelManager& ::globals::bubbleModels();
};
