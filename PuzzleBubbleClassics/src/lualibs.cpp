#include "lualibs.h"

#include "bubbles.h"
#include "globals.h"


namespace lua::lib
{
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

			Bubbles
		};

		static std::unordered_map<String, StandardLibId> StandardLibsRefs
		{
			{ "base", StandardLibId::Base },
			{ "coroutine", StandardLibId::Coroutine },
			{ "package", StandardLibId::Package },
			{ "string", StandardLibId::String },
			{ "utf8", StandardLibId::Utf8 },
			{ "table", StandardLibId::Table },
			{ "math", StandardLibId::Math },
			{ "io", StandardLibId::Io },
			{ "os", StandardLibId::Os },
			{ "debug", StandardLibId::Debug },
		};

		static void callOpenLuaStandardLib(LuaState* state, const LuaRef& env, lua_CFunction openFunction, const char* libname)
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
		}

		static bool openStandardLib(LuaState* state, const String& name)
		{
			using enum StandardLibId;

			if (!StandardLibsRefs.contains(name))
				return false;

			const LuaRef* env = globals::luaScripts().getCurrentRunScriptEnv();
			if (!env)
				return false;

			StandardLibId id = StandardLibsRefs[name];
			switch (id)
			{
				case Base: callOpenLuaStandardLib(state, *env, &luaopen_base, ""); return true;
			}

			return false;
		}

		static inline LuaScript getCurrentRunningScript()
		{
			return globals::luaScripts().getCurrentRunScript();
		}

		static LuaRef do_import(const std::string& spath, lua_State* state)
		{
			if (openStandardLib(state, spath))
			{
				LuaRef result(state);
				result.pop();
				return result;
			}
			else
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
	}

	void load_defaults(LuaState* state)
	{
		luabridge::getGlobalNamespace(state)
			.addFunction("import", &defs::do_import);
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
	namespace bubmodel
	{
		static BubbleModel* createModel(const std::string& name)
		{
			auto model = globals::bubbleModels().createModel(name);
			BubbleModel* ptr = model.get();
			return ptr;
		}
	}

	void load_bubble_models_import(LuaState* state)
	{
		luabridge::getGlobalNamespace(state)
			.beginNamespace("Bubbles")
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
	}
}


