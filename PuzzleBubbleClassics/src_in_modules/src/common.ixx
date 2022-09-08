module;

#include "headers.h"

export module Common;

export using Int8 = std::int8_t;
export using Int16 = std::int16_t;
export using Int32 = std::int32_t;
export using Int64 = std::int64_t;

export using UInt8 = std::uint8_t;
export using UInt16 = std::uint16_t;
export using UInt32 = std::uint32_t;
export using UInt64 = std::uint64_t;

export using Size = std::size_t;
export using Offset = std::size_t;

export using String = std::string;

export using Path = std::filesystem::path;

export using sf::Vector2f;
export using sf::Vector2i;
export using sf::Vector2u;

export namespace fs = std::filesystem;

export enum class PlayerId : UInt8 { First = 0, Second = 1, Single = First };
export enum class TimerMode : UInt8 { TURN = 0x1, END = 0x2, TURN_AND_END = TURN | END };

export namespace utils
{
	constexpr int game_canvas_with = 1920;
	constexpr int game_canvas_height = 1080;
}

export namespace utils
{
	template <typename _Ty = void>
	inline _Ty* malloc(Size size) { return reinterpret_cast<_Ty*>(::operator new(size)); }

	template <typename _Ty>
	inline _Ty* calloc(Size count, Size size) { return reinterpret_cast<_Ty*>(::operator new(size * count)); }

	inline void free(void* ptr) { ::operator delete(ptr); }

	template <typename _Ty, typename... _ArgTys> requires std::constructible_from<_Ty, _ArgTys...>
	constexpr _Ty& construct(_Ty& object, _ArgTys&&... args)
	{
		return *(new(std::addressof(object)) _Ty{ std::forward<_ArgTys>(args)... });
	}

	template <std::destructible _Ty>
	constexpr void destroy(_Ty& object) { object.~_Ty(); }

	template <std::copy_constructible _Ty>
	constexpr _Ty& copy_construct(_Ty& dst, const _Ty& src) { return construct(dst, src); }

	template <std::move_constructible _Ty>
	constexpr _Ty& move_construct(_Ty& dst, _Ty&& src) { return construct(dst, std::forward(src)); }

	template <std::integral _Ty>
	constexpr _Ty clamp(_Ty value, std::convertible_to<_Ty> auto min, std::convertible_to<_Ty> auto max)
	{
		return std::max(static_cast<_Ty>(min), std::min(static_cast<_Ty>(max), value));
	}

	template <typename _Ty>
	inline _Ty& from_string(const std::string& string, _Ty& value)
	{
		std::stringstream ss(string);
		ss >> value;
		return value;
	}

	template <typename _Ty>
	inline _Ty from_string(const std::string& string)
	{
		_Ty value;
		std::stringstream ss(string);
		ss >> value;
		return value;
	}

	constexpr bool is_integer(float value)
	{
		return value == static_cast<float>(static_cast<Int32>(value));
	}

	constexpr bool is_integer(double value)
	{
		return value == static_cast<double>(static_cast<Int64>(value));
	}

	template <std::integral _Ty>
	constexpr bool is_pair(_Ty value)
	{
		return (value % static_cast<_Ty>(value)) == 0;
	}

	inline Int64 systemTime()
	{
		return std::chrono::system_clock::now().time_since_epoch().count();
	}
}

export namespace utils::bit
{
	template <std::integral _Ty>
	constexpr _Ty set_to_one(_Ty base, unsigned int position)
	{
		return base | (static_cast<_Ty>(0x1) << position);
	}

	template <std::integral _Ty>
	constexpr _Ty set_to_zero(_Ty base, unsigned int position)
	{
		return base & ~(static_cast<_Ty>(0x1) << position);
	}

	template <std::integral _Ty>
	constexpr _Ty set(_Ty base, unsigned int position, bool state)
	{
		return state ? set_to_one<_Ty>(base, position) : set_to_zero<_Ty>(base, position);
	}

	template <std::integral _Ty>
	constexpr _Ty union_of(_Ty left, _Ty right)
	{
		return left | right;
	}

	template <std::integral _Ty>
	constexpr _Ty disjuntion_of(_Ty base, _Ty bits_to_remove)
	{
		return base & ~bits_to_remove;
	}

	template <std::integral _Ty>
	constexpr bool get(_Ty base, unsigned int position)
	{
		return ((base >> position) & static_cast<_Ty>(0x1)) == static_cast<_Ty>(0x1);
	}

	template <std::integral _Ty>
	constexpr bool check_all(_Ty base, _Ty bits)
	{
		return (base & bits) == bits;
	}

	template <std::integral _Ty>
	constexpr bool check_any(_Ty base, _Ty bits)
	{
		return (base & bits) != static_cast<_Ty>(0);
	}
}
