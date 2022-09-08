module;

#include "../headers.h"
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

export module Lua:libs;

import :scripts;
import :raw;

import Common;


namespace lua::lib
{
	export void loadDefaults(LuaState* state);
}

namespace lua::lib
{
	std::pair<bool, String> load_utils_lib(LuaState* state);
	std::pair<bool, String> load_bubble_models_import(LuaState* state);
}



// IMPLEMENTATION //

namespace lua::lib
{
	namespace name
	{
		static constexpr const char pbcid[] = "PBC";
		static constexpr String pbcname(const char* name) { return String(pbcid) + "." + String(name); }

		static constexpr const char bubbles[] = "bubbles";
		static constexpr const char utils[] = "utils";
	}

	namespace defs
	{
		enum class StandardLibId
		{
			Base,
			Coroutine,
			Package,
			String,
			Utf8,
			Table,
			Math,
			Io,
			Os,
			Debug,

			PBC_Utils,
			PBC_Bubbles
		};

		static const std::unordered_map<String, StandardLibId> StandardLibsRefs
		{
			{ "base", StandardLibId::Base },
			{ LUA_COLIBNAME, StandardLibId::Coroutine },
			{ LUA_LOADLIBNAME, StandardLibId::Package },
			{ LUA_STRLIBNAME, StandardLibId::String },
			{ LUA_UTF8LIBNAME, StandardLibId::Utf8 },
			{ LUA_TABLIBNAME, StandardLibId::Table },
			{ LUA_MATHLIBNAME, StandardLibId::Math },
			{ LUA_IOLIBNAME, StandardLibId::Io },
			{ LUA_OSLIBNAME, StandardLibId::Os },
			{ LUA_DBLIBNAME, StandardLibId::Debug },

			{ lib::name::pbcname(lib::name::utils), StandardLibId::PBC_Utils },
			{ lib::name::pbcname(lib::name::bubbles), StandardLibId::PBC_Bubbles }
		};

		static bool callOpenLuaStandardLib(LuaState* state, const LuaRef& env, lua_CFunction openFunction, const char* libname)
		{
			lua_pushcfunction(state, &luaopen_base);
			lua_pushstring(state, "");
			lua_call(state, 1, 1);

			luabridge::push(state, luabridge::Nil());
			while (lua_next(state, -2))
			{
				LuaRef key = LuaRef::fromStack(state, -2);
				LuaRef val = LuaRef::fromStack(state, -1);

				env[key] = val;

				lua_pop(state, 1);
			}

			lua_pop(state, 1);
			return true;
		}

		static bool callOpenBpsLib(LuaState* state, const LuaRef& env, std::pair<bool, String>(*openFunction)(LuaState*))
		{
			auto result = openFunction(state);
			if (result.first)
			{
				LuaRef bps = env[lib::name::pbcid];
				if (!bps.isTable())
				{
					env[lib::name::pbcid] = LuaRef::newTable(state);
					bps = env[lib::name::pbcid];
				}

				bps[result.second] = luabridge::getGlobal(state, result.second.c_str());
				return true;
			}
			return false;
		}

		static bool openStandardLib(const std::string& name, lua_State* state)
		{
			using enum StandardLibId;

			if (!StandardLibsRefs.contains(name))
				return false;

			const LuaRef* env = globals::luaScripts().getCurrentRunScriptEnv();
			if (!env)
				return false;

			StandardLibId id = StandardLibsRefs.at(name);
			switch (id)
			{
				case Base: return callOpenLuaStandardLib(state, *env, &luaopen_base, "");
				case Coroutine: return callOpenLuaStandardLib(state, *env, &luaopen_coroutine, LUA_COLIBNAME);
				case Package: return callOpenLuaStandardLib(state, *env, &luaopen_package, LUA_LOADLIBNAME);
				case String: return callOpenLuaStandardLib(state, *env, &luaopen_string, LUA_STRLIBNAME);
				case Utf8: return callOpenLuaStandardLib(state, *env, &luaopen_utf8, LUA_UTF8LIBNAME);
				case Table: return callOpenLuaStandardLib(state, *env, &luaopen_table, LUA_TABLIBNAME);
				case Math: return callOpenLuaStandardLib(state, *env, &luaopen_math, LUA_MATHLIBNAME);
				case Io: return callOpenLuaStandardLib(state, *env, &luaopen_io, LUA_IOLIBNAME);
				case Os: return callOpenLuaStandardLib(state, *env, &luaopen_os, LUA_OSLIBNAME);
				case Debug: return callOpenLuaStandardLib(state, *env, &luaopen_debug, LUA_DBLIBNAME);

				case PBC_Utils: return callOpenBpsLib(state, *env, &load_utils_lib);
				//case PBC_Bubbles: return callOpenBpsLib(state, *env, &load_bubble_models_import);
			}

			return false;
		}

		static inline LuaScript getCurrentRunningScript()
		{
			return globals::luaScripts().getCurrentRunScript();
		}

		static LuaRef importScript(const std::string& spath, lua_State* state)
		{
			Path path(spath);
			if (!path.is_absolute())
			{
				LuaScript currentScript = getCurrentRunningScript();
				if (!currentScript)
					return LuaRef(state);

				path = std::filesystem::absolute(currentScript.getDirectory() / path);
			}

			if (!path.has_extension())
				path += ".lua";

			LuaScript script = globals::luaScripts().getScript(path);
			if (!script)
				return LuaRef(state);

			LuaRef env = LuaRef::newTable(state);
			script(env);

			return env;
		}
	}



	void loadDefaults(LuaState* state)
	{
		luabridge::getGlobalNamespace(state)
			.addFunction(name::Import, &defs::importScript)
			.addFunction(name::OpenLib, &defs::openStandardLib);
	}
}



namespace lua::lib
{
	namespace vec2f
	{
		sf::Vector2f ctor(LuaRef cls, LuaRef lx, LuaRef ly)
		{
			if (lx.isNil())
				return {};

			float x = lx.cast<float>();
			if (ly.isNil())
				return { x, x };

			float y = ly.cast<float>();
			return { x, y };
		}
		bool eq(sf::Vector2f* self, const sf::Vector2f& v)
		{
			return *self == v;
		}
		sf::Vector2f add(sf::Vector2f* self, const sf::Vector2f& v)
		{
			return *self + v;
		}
		sf::Vector2f sub(sf::Vector2f* self, const sf::Vector2f& v)
		{
			return *self - v;
		}
		sf::Vector2f mul(sf::Vector2f* self, float v)
		{
			return *self * v;
		}
		sf::Vector2f div(sf::Vector2f* self, float v)
		{
			return *self / v;
		}
		sf::Vector2f copy(sf::Vector2f* self)
		{
			return *self;
		}
		String print(sf::Vector2f* self)
		{
			std::stringstream ss;
			ss << "(" << self->x << ", " << self->y << ")";
			return ss.str();
		}
	}
	void register_vec2f_class(LuaState* state)
	{
		luabridge::getGlobalNamespace(state)
			.beginClass<sf::Vector2f>("Vector2f")
			.addData("x", &sf::Vector2f::x)
			.addData("y", &sf::Vector2f::y)
			.addConstructor<void (*) (float, float)>()
			.addFunction(meta::eq, &vec2f::eq)
			.addFunction(meta::add, &vec2f::add)
			.addFunction(meta::sub, &vec2f::sub)
			.addFunction(meta::mul, &vec2f::mul)
			.addFunction(meta::div, &vec2f::div)
			.addFunction(meta::tostring, &vec2f::print)
			.addFunction("copy", &vec2f::copy)
			.addStaticFunction(meta::call, &vec2f::ctor) //constructor
			.endClass();
	}
}



namespace lua::lib
{
	namespace utilslib
	{
		namespace files
		{
			static void for_each_file_in_directory(const std::string& sdir, LuaRef action)
			{
				Path dir(sdir);
				if (fs::is_directory(dir))
				{
					for (const auto& entry : fs::directory_iterator(dir))
					{
						if (entry.exists())
							action(entry.path().string());
					}
				}
			}

			static bool is_directory(const std::string& sdir)
			{
				return fs::is_directory(sdir);
			}

			static bool is_file(const std::string& sdir)
			{
				return fs::is_regular_file(sdir);
			}

			static bool exists(const std::string& sdir)
			{
				return fs::exists(sdir);
			}

			static std::string filename(const std::string& sdir)
			{
				return Path(sdir).filename().string();
			}
		}

		static std::string current_script_path()
		{
			auto script = globals::luaScripts().getCurrentRunScript();
			if (!script)
				return "";
			return script.getPath().string();
		}

		static std::string current_script_directory()
		{
			auto script = globals::luaScripts().getCurrentRunScript();
			if (!script)
				return "";
			return script.getDirectory().string();
		}
	}

	std::pair<bool, String> load_utils_lib(LuaState* state)
	{
		luabridge::getGlobalNamespace(state)
			.beginNamespace(name::utils)
			.beginNamespace("files")
			.addFunction("forEachEntryInDirectory", &utilslib::files::for_each_file_in_directory)
			.addFunction("isDirectory", &utilslib::files::is_directory)
			.addFunction("isFile", &utilslib::files::is_file)
			.addFunction("exists", &utilslib::files::exists)
			.addFunction("filename", &utilslib::files::filename)
			.endNamespace()

			.addFunction("getCurrentScriptPath", &utilslib::current_script_path)
			.addFunction("getCurrentScriptDirectory", &utilslib::current_script_directory)
			.endNamespace();

		return { true, name::utils };
	}
}



namespace lua::lib
{
	/*namespace bubmodel
	{
		static BubbleModel* createModel(const std::string& name)
		{
			auto model = globals::bubbleModels().createModel(name);
			BubbleModel* ptr = model.get();
			return ptr;
		}
	}

	std::pair<bool, String> load_bubble_models_import(LuaState* state)
	{
		luabridge::getGlobalNamespace(state)
			.beginNamespace(name::bubbles)
			.beginClass<BubbleModel>("BubbleModel")
			.addData("name", &BubbleModel::name, false)
			.addData("colorless", &BubbleModel::colorless)
			.addData("multicolor", &BubbleModel::multicolor)
			.addData("floating", &BubbleModel::floating)
			.addData("destroyInBottom", &BubbleModel::destroyInBottom)
			.addData("requireDestroyToClear", &BubbleModel::requireDestroyToClear)
			.addData("onlyBoardColorInArrowGen", &BubbleModel::onlyBoardColorInArrowGen)
			.addProperty("resistence", &BubbleModel::resistence)
			.addProperty("pointsOfTurnsToDown", &BubbleModel::pointsOfTurnsToDown)

			.addProperty("onInit", &BubbleModel::lua_GetOnInit, &BubbleModel::lua_SetOnInit)
			.addProperty("onCollide", &BubbleModel::lua_GetOnCollide, &BubbleModel::lua_SetOnCollide)
			.addProperty("onInserted", &BubbleModel::lua_GetOnInserted, &BubbleModel::lua_SetOnInserted)
			.addProperty("onExplode", &BubbleModel::lua_GetOnExplode, &BubbleModel::lua_SetOnExplode)
			.addProperty("onNeighborInserted", &BubbleModel::lua_GetOnNeighborInserted, &BubbleModel::lua_SetOnNeighborInserted)
			.addProperty("onNeighborExplode", &BubbleModel::lua_GetOnNeighborExplode, &BubbleModel::lua_SetOnNeighborExplode)

			.addStaticFunction("create", &bubmodel::createModel)
			.endClass()
			.endNamespace();

		return { true, name::bubbles };
	}*/
}
