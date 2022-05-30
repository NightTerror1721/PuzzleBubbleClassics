#include "lua.h"

#include <lua/lua.hpp>

namespace lua
{
	static String to_string(LuaState* state, int index)
	{
		Size len;
		const char* cstr = lua_tolstring(state, index, &len);
		return String(cstr, len);
	}

	static int catch_error(LuaState* state, int status, String* error_msg)
	{
		if (status != LUA_OK)
		{
			if (error_msg)
			{
				*error_msg = to_string(state, -1);
				std::cerr << *error_msg << std::endl;
			}
			else
			{
				std::cerr << lua_tostring(state, -1) << std::endl;
			}
		}
		return status;
	}
}

namespace lua
{
	LuaState* new_state() { return luaL_newstate(); }

	void close_state(LuaState* state) { lua_close(state); }

	void open_default_libs(LuaState* state)
	{
		luaL_openlibs(state);
	}



	

	int load_file(LuaState* state, const char* filename, String* error_msg)
	{
		return catch_error(state, luaL_loadfile(state, filename), error_msg);
	}

	int load_file(LuaState* state, const String& filename, String* error_msg)
	{
		return catch_error(state, luaL_loadfile(state, filename.c_str()), error_msg);
	}

	int load_file(LuaState* state, const Path& filename, String* error_msg)
	{
		return catch_error(state, luaL_loadfile(state, filename.string().c_str()), error_msg);
	}

	int runfile(LuaState* state, const char* filename, String* error_msg)
	{
		int status = load_file(state, filename, error_msg);
		if (status == LUA_OK)
			status = catch_error(state, lua_pcall(state, 0, LUA_MULTRET, 0), error_msg);
		return status;
	}

	int runfile(LuaState* state, const String& filename, String* error_msg)
	{
		int status = load_file(state, filename, error_msg);
		if (status == LUA_OK)
			status = catch_error(state, lua_pcall(state, 0, LUA_MULTRET, 0), error_msg);
		return status;
	}

	int runfile(LuaState* state, const Path& filename, String* error_msg)
	{
		int status = load_file(state, filename, error_msg);
		if (status == LUA_OK)
			status = catch_error(state, lua_pcall(state, 0, LUA_MULTRET, 0), error_msg);
		return status;
	}
}

/*namespace lua
{
	std::ostream& State::stack_dump(std::ostream& os, bool add_endl)
	{
		int top = get_top();
		for (int i = 1; i <= top; ++i)
		{
			if (i > 1)
				os << ", ";

			Type type = this->type(i);
			switch (type)
			{
			case Type::String:
				os << '"' << to_string(i) << '"';
				break;

			case Type::Boolean:
				os << (to_bool(i) ? "true" : "false");
				break;

			case Type::Number:
				os << to_number<lua_Number>(i);
				break;

			default:
				os << type_name(i);
				break;
			}
		}

		if (add_endl)
			os << std::endl;

		return os;
	}


	static inline Status catch_error(State& state, Status status, String* error_msg)
	{
		if (status != Status::Ok)
		{
			if (error_msg)
			{
				*error_msg = state.to_string(-1);
				std::cerr << *error_msg << std::endl;
			}
			else
			{
				std::cerr << state.to_cstring(-1) << std::endl;
			}
		}
		return status;
	}

	Status State::load_file(const char* filename, String* error_msg)
	{
		return catch_error(*this, static_cast<Status>(luaL_loadfile(_state, filename)), error_msg);
	}
	Status State::load_file(const String& filename, String* error_msg)
	{
		return catch_error(*this, static_cast<Status>(luaL_loadfile(_state, filename.c_str())), error_msg);
	}
	Status State::load_file(const Path& filename, String* error_msg)
	{
		return catch_error(*this, static_cast<Status>(luaL_loadfile(_state, filename.string().c_str())), error_msg);
	}

	Status State::runfile(const char* filename, String* error_msg)
	{
		Status status = load_file(filename, error_msg);
		if (status == Status::Ok)
			status = catch_error(*this, static_cast<Status>(lua_pcall(_state, 0, LUA_MULTRET, 0)), error_msg);
		return status;
	}
	Status State::runfile(const String& filename, String* error_msg)
	{
		Status status = load_file(filename, error_msg);
		if (status == Status::Ok)
			status = catch_error(*this, static_cast<Status>(lua_pcall(_state, 0, LUA_MULTRET, 0)), error_msg);
		return status;
	}
	Status State::runfile(const Path& filename, String* error_msg)
	{
		Status status = load_file(filename, error_msg);
		if (status == Status::Ok)
			status = catch_error(*this, static_cast<Status>(lua_pcall(_state, 0, LUA_MULTRET, 0)), error_msg);
		return status;
	}


	ErrorBuilder& operator<< (ErrorBuilder& os, raise_t)
	{
		os._state->push_string(os.str());
		os._state->error();
		return os;
	}

	int State::raise_field_not_found(const char* classname, const char* fieldname)
	{
		new_error() << classname << " has not any field with name '" << fieldname << "'" << lua::raise;
		return 1;
	}

	void State::foreach_table_elem(Index table_index, const std::function<void(Index, Index)>& action)
	{
		push_nil();
		while (lua_next(_state, table_index))
		{
			action(1_lit, 0_lit);
			pop(1);
		}
	}

	void State::foreach_arraytable_elem(Index table_index, const std::function<void(Index, Type)>& action)
	{
		Size len = rawlen(table_index);
		for (Offset i = 1; i <= len; ++i)
		{
			action(0_lit, rawget_index(table_index, i));
			pop(1);
		}
	}

	/*void State::register_class(
		const char* name,
		CFunction constructor,
		CFunction destructor,
		const std::vector<Register>& methods,
		const std::vector<Register>& metamethods,
	)
	{
		Size methodsSize = methods.size() + (constructor ? 2 : 1);

		Register* regs = new Register[methodsSize];
		std::memcpy(regs, methods.data(), methods.size() * sizeof(Register));
		regs[methodsSize - 1] = { .name = NULL, .func = NULL };
		if(constructor)
			regs[methodsSize - 2] = { .name = "new", .func = constructor };

		new_table();
		Index libIdx = get_top();

		new_metatable(name);
		Index metaIdx = get_top();
		luaL_setfuncs(_state, regs, 0);

		//newlib()
	}

	class Test
	{
	private:
		lua_State* _state;
	};
}*/
