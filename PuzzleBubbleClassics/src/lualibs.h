#pragma once

#include "lua.h"

namespace lua::lib
{
	void register_vec2f_class(LuaState* state);

	void load_bubble_models_import(LuaState* state);
}
