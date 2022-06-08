#include "props.h"
#include "json.h"
#include "resources.h"
#include "props.h"
#include "fonts.h"
#include "lualibs.h"
#include "game_basics.h"
#include "game_controller.h"



int main(int argc, char** argv)
{
	globals::fonts().loadAll();

	//globals::game.start();

	LuaState* state = lua::get_default_state();

	lua::open_default_libs(state);
	lua::lib::load_bubble_models_import(state);
	lua::lib::load_defaults(state);

	auto script = lua::get_script("test_scripts/test.lua");


	script.setEnvValuesFromGlobal("Bubbles");
	script();

	script.getEnv().print(std::cout);
	script.getEnvValue("name").print(std::cout);


	/*Json json = {
		{ "name", "Krampus" },
		{ "power", 50 },
		{ "dash", true }
	};

	Json::object_t map;
	map.emplace("part", false);

	json.emplace("hola", map);

	utils::json::write("test.json"_p, json);
	utils::json::write(std::cout, json, 4);


	gprops::load();

	gprops::dump(std::cout, 4);

	gprops::store();*/


	/*lua_State* lstate = luaL_newstate();

	LuaState state;
	*reinterpret_cast<lua_State**>(std::addressof(state)) = lstate;

	lua_close(lstate);*/

	return 0;
}
