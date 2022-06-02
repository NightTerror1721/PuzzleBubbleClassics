#include "lualibs.h"

#include "bubbles.h"
#include "globals.h"

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
			auto model = globals::bubblesManager.createModel(name);
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


