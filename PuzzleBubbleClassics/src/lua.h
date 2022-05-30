#pragma once

#include "common.h"

//#include <lua/lua.hpp>
//#include <LuaBridge/LuaBridge.h>

typedef struct lua_State LuaState;

namespace lua
{
	LuaState* new_state();
	void close_state(LuaState* state);

	void open_default_libs(LuaState* state);

	int load_file(LuaState* state, const char* filename, String* error_msg = nullptr);
	int load_file(LuaState* state, const String& filename, String* error_msg = nullptr);
	int load_file(LuaState* state, const Path& filename, String* error_msg = nullptr);

	int runfile(LuaState* state, const char* filename, String* error_msg = nullptr);
	int runfile(LuaState* state, const String& filename, String* error_msg = nullptr);
	int runfile(LuaState* state, const Path& filename, String* error_msg = nullptr);
}

namespace lua::meta
{
	constexpr const char* add = "__add";
	constexpr const char* sub = "__sub";
	constexpr const char* mul = "__mul";
	constexpr const char* div = "__div";
	constexpr const char* mod = "__mod";
	constexpr const char* pow = "__pow";
	constexpr const char* unm = "__unm";
	constexpr const char* idiv = "__idiv";
	constexpr const char* band = "__band";
	constexpr const char* bor = "__bor";
	constexpr const char* bxor = "__bxor";
	constexpr const char* bnot = "__bnot";
	constexpr const char* shl = "__shl";
	constexpr const char* shr = "__shr";
	constexpr const char* concat = "__concat";
	constexpr const char* len = "__len";
	constexpr const char* eq = "__eq";
	constexpr const char* lt = "__lt";
	constexpr const char* le = "__le";
	constexpr const char* index = "__index";
	constexpr const char* newindex = "__newindex";
	constexpr const char* call = "__call";
	constexpr const char* tostring = "__tostring";
	constexpr const char* gc = "__gc";
	constexpr const char* close = "__close";
	constexpr const char* mode = "__mode";
	constexpr const char* name = "__name";
}



/*#include "common.h"

namespace lua
{
	typedef luaL_Reg Register;
	typedef lua_CFunction CFunction;

	enum class Type
	{
		None = LUA_TNONE,
		Nil = LUA_TNIL,
		Boolean = LUA_TBOOLEAN,
		LightUserdata = LUA_TLIGHTUSERDATA,
		Number = LUA_TNUMBER,
		String = LUA_TSTRING,
		Table = LUA_TTABLE,
		Function = LUA_TFUNCTION,
		Userdata = LUA_TUSERDATA,
		Thread = LUA_TTHREAD
	};

	enum class Status
	{
		Ok = LUA_OK,
		Yield = LUA_YIELD,
		RuntimeError = LUA_ERRRUN,
		SyntaxError = LUA_ERRSYNTAX,
		MemoryError = LUA_ERRMEM,
		ErrorManagerFunctionError = LUA_ERRERR,
		FileOpenError = LUA_ERRFILE
	};

	namespace meta
	{
		constexpr const char* add = "__add";
		constexpr const char* sub = "__sub";
		constexpr const char* mul = "__mul";
		constexpr const char* div = "__div";
		constexpr const char* mod = "__mod";
		constexpr const char* pow = "__pow";
		constexpr const char* unm = "__unm";
		constexpr const char* idiv = "__idiv";
		constexpr const char* band = "__band";
		constexpr const char* bor = "__bor";
		constexpr const char* bxor = "__bxor";
		constexpr const char* bnot = "__bnot";
		constexpr const char* shl = "__shl";
		constexpr const char* shr = "__shr";
		constexpr const char* concat = "__concat";
		constexpr const char* len = "__len";
		constexpr const char* eq = "__eq";
		constexpr const char* lt = "__lt";
		constexpr const char* le = "__le";
		constexpr const char* index = "__index";
		constexpr const char* newindex = "__newindex";
		constexpr const char* call = "__call";
		constexpr const char* tostring = "__tostring";
		constexpr const char* gc = "__gc";
		constexpr const char* close = "__close";
		constexpr const char* mode = "__mode";
		constexpr const char* name = "__name";
	}

	constexpr int raw_type(Type type) { return static_cast<int>(type); }

	constexpr bool operator! (Status right) { return right != Status::Ok; }

	class Index
	{
	private:
		int _index;

	private:
		constexpr Index _sum(std::integral auto value) const
		{
			if (_index < 0)
				return _index - value;
			return _index + value;
		}

		constexpr Index _subtract(std::integral auto value) const
		{
			if (_index < 0)
				return std::min(-1, _index + value);
			return std::max(0, _index - value);
		}

	public:
		constexpr Index() : _index(0) {}
		constexpr Index(int idx) : _index(idx) {}
		constexpr Index(const Index&) = default;
		constexpr ~Index() = default;

		constexpr Index& operator= (int idx) { return _index = idx, *this; }
		constexpr Index& operator= (const Index&) = default;

		constexpr operator int() const { return _index; }

		constexpr bool is_top_section() const { return _index < 0; }

		constexpr Index operator+ (int right) const
		{
			return right < 0 ? _subtract(right) : _sum(right);
		}
		constexpr Index operator- (int right) const
		{
			return right < 0 ? _sum(right) : _subtract(right);
		}

		constexpr Index& operator+= (int right)
		{
			return _index = (*this) + right, *this;
		}
		constexpr Index& operator-= (int right)
		{
			return _index = (*this) - right, *this;
		}
	};

	constexpr Index top = -1;
	constexpr Index bottom = 1;
}

constexpr lua::Index operator"" _lit(unsigned long long value) { return lua::top + static_cast<int>(value - 1); }
constexpr lua::Index operator"" _lib(unsigned long long value) { return lua::bottom + static_cast<int>(value - 1); }

namespace lua
{
	class ErrorBuilder;

	class State
	{
	private:
		lua_State* _state;
		bool _wrapped;

	public:
		inline State() : _state(luaL_newstate()), _wrapped(false) {}
		inline State(State&& right) noexcept : _state(right._state), _wrapped(right._wrapped) { right._state = nullptr, right._wrapped = false; }
		inline ~State()
		{
			if(_wrapped)
				lua_close(_state);
			_state = nullptr;
			_wrapped = false;
		}

		inline State& operator= (State&& right) noexcept
		{
			if (_state)
				this->~State();
			_state = right._state;
			_wrapped = right._wrapped;
			right._state = nullptr;
			right._wrapped = false;
			return *this;
		}

		State(const State&) = delete;
		State& operator= (const State&) = delete;


	private:
		inline explicit State(lua_State* lstate) : _state(lstate), _wrapped(true) {}

	public:
		static inline State wrap(lua_State* lstate)
		{
			return State(lstate);
		}


		std::ostream& stack_dump(std::ostream& os, bool add_endl = true);
		inline std::ostream& stack_dump(bool add_endl = true) { return stack_dump(std::cout, add_endl); }

		friend inline std::ostream& operator<< (std::ostream& left, State& right) { return right.stack_dump(left, false); }

		inline void openlibs() { luaL_openlibs(_state); }

		inline void newlib(const Register* reg)
		{
#pragma warning(push)
#pragma warning(disable: 6384)
#pragma warning(disable: 4309)
			luaL_newlib(_state, reg);
#pragma warning(pop)
		}


		inline void push_nil() { lua_pushnil(_state); }
		inline void push_bool(bool value) { lua_pushboolean(_state, value); }
		inline void push_string(const char* str, Size len) { lua_pushlstring(_state, str, len); }
		inline void push_string(const char* str) { lua_pushstring(_state, str); }
		inline void push_string(const String& str) { lua_pushlstring(_state, str.c_str(), str.size()); }

		inline void push_cfunction(CFunction function) { lua_pushcfunction(_state, function); }
		inline void push_cclosure(CFunction function, int n_args) { lua_pushcclosure(_state, function, n_args); }

		template<typename _Ty>
			requires std::integral<_Ty>&& std::convertible_to<_Ty, lua_Integer>
		inline void push_int(_Ty value) { lua_pushinteger(_state, static_cast<lua_Integer>(value)); }

		template<typename _Ty> requires
			(std::integral<_Ty>&& std::convertible_to<_Ty, lua_Integer>) ||
			(std::floating_point<_Ty> && std::convertible_to<_Ty, lua_Number>)
		inline void push_number(_Ty value)
		{
			if constexpr (std::integral<_Ty>)
			{
				lua_pushinteger(_state, static_cast<lua_Integer>(value));
			}
			else
			{
				lua_pushnumber(_state, static_cast<lua_Number>(value));
			}
		}

		inline void push_lightuserdata(void* value) { lua_pushlightuserdata(_state, value); }

		inline int error() { return lua_error(_state); }

		template<Type _LuaType>
		inline bool is(Index index) const
		{
			if constexpr (_LuaType == Type::Nil) { return lua_isnil(_state, index); }
			else if constexpr (_LuaType == Type::Boolean) { return lua_isboolean(_state, index); }
			else if constexpr (_LuaType == Type::LightUserdata) { return lua_islightuserdata(_state, index); }
			else if constexpr (_LuaType == Type::Number) { return lua_isnumber(_state, index); }
			else if constexpr (_LuaType == Type::String) { return lua_isstring(_state, index); }
			else if constexpr (_LuaType == Type::Table) { return lua_istable(_state, index); }
			else if constexpr (_LuaType == Type::Function) { return lua_isfunction(_state, index); }
			else if constexpr (_LuaType == Type::Userdata) { return lua_isuserdata(_state, index); }
			else if constexpr (_LuaType == Type::Thread) { return lua_isthread(_state, index); }
			else
			{
				//static_assert(false, "Invalid LuaType");
				return false;
			}
		}

		inline Type type(Index index) const { return static_cast<Type>(lua_type(_state, index)); }

		inline String type_name(Index index) const { return String(luaL_typename(_state, index)); }
		inline String type_name(Type type) const { return String(lua_typename(_state, static_cast<int>(type))); }

		inline int get_top() { return lua_gettop(_state); }
		inline void set_top(Index index) { lua_settop(_state, index); }
		inline void push_value(Index index) { lua_pushvalue(_state, index); }
		inline void remove(Index index) { lua_remove(_state, index); }
		inline void insert(Index index) { lua_insert(_state, index); }
		inline void replace(Index index) { lua_replace(_state, index); }
		inline void pop(unsigned int index = 1) { lua_pop(_state, static_cast<int>(index)); }

		inline bool to_bool(Index index) const { return lua_toboolean(_state, index); }
		inline const char* to_cstring(Index index, Size& len) const { return lua_tolstring(_state, index, &len); }
		inline const char* to_cstring(Index index) const { return lua_tostring(_state, index); }
		inline String to_string(Index index) const
		{
			Size len;
			const char* cstr = lua_tolstring(_state, index, &len);
			return String(cstr, len);
		}

		inline CFunction to_cfunction(Index index) const { return lua_tocfunction(_state, index); }

		template<typename _Ty>
			requires std::integral<_Ty>&& std::convertible_to<_Ty, lua_Integer>
		inline _Ty to_int(Index index) const
		{
			return static_cast<_Ty>(lua_tointeger(_state, index));
		}

		template<typename _Ty>
			requires std::integral<_Ty>&& std::convertible_to<_Ty, lua_Integer>
		inline _Ty to_int(Index index, bool& is_int) const
		{
			return static_cast<_Ty>(lua_tointegerx(_state, index, reinterpret_cast<int*>(&is_int)));
		}

		template<typename _Ty> requires
			(std::integral<_Ty>&& std::convertible_to<_Ty, lua_Integer>) ||
			(std::floating_point<_Ty> && std::convertible_to<_Ty, lua_Number>)
		inline _Ty to_number(Index index) const
		{
			return static_cast<_Ty>(lua_tonumber(_state, index));
		}

		template<typename _Ty> requires
			(std::integral<_Ty>&& std::convertible_to<_Ty, lua_Integer>) ||
			(std::floating_point<_Ty> && std::convertible_to<_Ty, lua_Number>)
		inline _Ty to_number(Index index, bool& is_number) const
		{
			return static_cast<_Ty>(lua_tonumberx(_state, index, reinterpret_cast<int*>(&is_number)));
		}

		template<typename _Ty>
		inline _Ty* to_userdata(Index index) const
		{
			return reinterpret_cast<_Ty*>(lua_touserdata(_state, index));
		}

		template<typename _Ty = void>
		inline const _Ty* to_pointer(Index index) const
		{
			return reinterpret_cast<const _Ty*>(lua_topointer(_state, index));
		}


		inline void check_any(int arg) const { luaL_checkany(_state, arg); }

		template<typename _Ty> requires
			(std::integral<_Ty>&& std::convertible_to<_Ty, lua_Integer>) ||
			(std::floating_point<_Ty> && std::convertible_to<_Ty, lua_Number>)
		inline _Ty check_number(int arg) const
		{
			if constexpr (std::integral<_Ty>)
			{
				return static_cast<_Ty>(luaL_checkinteger(_state, arg));
			}
			else
			{
				return static_cast<_Ty>(luaL_checknumber(_state, arg));
			}
		}

		inline const char* check_clstring(int arg, Size& len) const { return luaL_checklstring(_state, arg, &len); }
		inline const char* check_cstring(int arg) const { return luaL_checkstring(_state, arg); }
		inline String check_string(int arg) const
		{
			Size len;
			const char* cstr = luaL_checklstring(_state, arg, &len);
			return len > 0 && cstr ? String(cstr, len) : String();
		}

		inline void check_stack(int sz, const char* msg) const { luaL_checkstack(_state, sz, msg); }

		inline void check_type(int arg, Type type) const { luaL_checktype(_state, arg, static_cast<int>(type)); }

		template<typename _Ty>
		_Ty* check_udata(int arg, const char* type_name) const
		{
			return reinterpret_cast<_Ty*>(luaL_checkudata(_state, arg, type_name));
		}

		inline void get_field(Index index, const char* key) { lua_getfield(_state, index, key); }
		inline void get_field(Index index, const String& key) { lua_getfield(_state, index, key.c_str()); }

		inline void get_global(const char* name) { lua_getglobal(_state, name); }
		inline void get_global(const String& name) { lua_getglobal(_state, name.c_str()); }

		inline void get_table(Index index) { lua_gettable(_state, index); }

		inline int get_metatable(Index index) { lua_getmetatable(_state, index); }

		inline void set_field(Index index, const char* key) { lua_setfield(_state, index, key); }
		inline void set_field(Index index, const String& key) { lua_setfield(_state, index, key.c_str()); }

		inline void set_global(const char* name) { lua_setglobal(_state, name); }
		inline void set_global(const String& name) { lua_setglobal(_state, name.c_str()); }

		inline void set_table(Index index) { lua_settable(_state, index); }

		inline int set_metatable(Index index) { lua_setmetatable(_state, index); }

		inline void set_metatable(const char* tablename) { luaL_setmetatable(_state, tablename); }

		template<typename _Ty>
		_Ty* new_userdata(Size nbytes) { return reinterpret_cast<_Ty*>(lua_newuserdata(_state, nbytes)); }

		template<typename _Ty>
		_Ty* new_userdata() { return reinterpret_cast<_Ty*>(lua_newuserdata(_state, sizeof(_Ty))); }

		inline Status protected_call(int nargs, int nresults, String* error_msg = nullptr)
		{
			Status status = static_cast<Status>(lua_pcall(_state, nargs, nresults, 0));
			if (status != Status::Ok && error_msg)
				*error_msg = to_string(-1);
			return status;
		}

		inline void call(int nargs, int nresults)
		{
			lua_call(_state, nargs, nresults);
		}

		Status load_file(const char* filename, String* error_msg = nullptr);
		Status load_file(const String& filename, String* error_msg = nullptr);
		Status load_file(const Path& filename, String* error_msg = nullptr);

		Status runfile(const char* filename, String* error_msg = nullptr);
		Status runfile(const String& filename, String* error_msg = nullptr);
		Status runfile(const Path& filename, String* error_msg = nullptr);

		inline void set_to_table(Index table_idx, const char* name, CFunction function, bool pop_table = false)
		{
			push_string(name);
			push_cfunction(function);
			set_table(table_idx);
			if (pop_table)
				this->remove(table_idx);
		}

		inline void new_table() { lua_newtable(_state); }

		inline bool new_metatable(const char* tablename) { return luaL_newmetatable(_state, tablename); }

		inline const char* set_upvalue(Index function_index, int upvalue_num) { return lua_setupvalue(_state, function_index, upvalue_num); }
		inline const char* get_upvalue(Index function_index, int upvalue_num) { return lua_getupvalue(_state, function_index, upvalue_num); }

		void foreach_table_elem(Index table_index, const std::function<void(Index, Index)>& action);

		inline Size rawlen(Index index) const
		{
			return lua_rawlen(_state, index);
		}

		inline void rawset_index(Index index, lua_Integer offset)
		{
			lua_rawseti(_state, index, offset);
		}

		inline Type rawget_index(Index index, lua_Integer offset)
		{
			return static_cast<Type>(lua_rawgeti(_state, index, offset));
		}

		void foreach_arraytable_elem(Index table_index, const std::function<void(Index, Type)>& action);

		ErrorBuilder new_error();
		int raise_field_not_found(const char* classname, const char* fieldname);
		int raise_field_not_found(const char* classname, const String& fieldname);
		int raise_field_not_found(const String& classname, const String& fieldname);
	};
}

namespace lua
{
	struct raise_t {};
	constexpr raise_t raise = {};

	class ErrorBuilder : public std::ostringstream
	{
	private:
		State* _state;

	public:
		inline ErrorBuilder(State* state) : std::ostringstream(), _state{ state } {}
		ErrorBuilder(const ErrorBuilder&) = default;
		ErrorBuilder(ErrorBuilder&&) noexcept = default;
		~ErrorBuilder() = default;

		ErrorBuilder& operator= (const ErrorBuilder&) = default;
		ErrorBuilder& operator= (ErrorBuilder&&) noexcept = default;

		friend ErrorBuilder& operator<< (ErrorBuilder& os, raise_t);
	};

	inline ErrorBuilder State::new_error() { return ErrorBuilder(this); }

	inline int State::raise_field_not_found(const char* classname, const String& fieldname)
	{
		return raise_field_not_found(classname, fieldname.c_str());
	}
	inline int State::raise_field_not_found(const String& classname, const String& fieldname)
	{
		return raise_field_not_found(classname.c_str(), fieldname.c_str());
	}
}*/
