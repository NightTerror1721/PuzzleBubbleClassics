#pragma once

#include "lua.h"

namespace lua::lib
{
	void load_defaults(LuaState* state);

	void register_vec2f_class(LuaState* state);

	std::pair<bool, String> load_utils_lib(LuaState* state);
	std::pair<bool, String> load_bubble_models_import(LuaState* state);
}
