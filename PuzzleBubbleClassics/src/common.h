#pragma once

#include <unordered_map>
#include <filesystem>
#include <functional>
#include <algorithm>
#include <exception>
#include <iostream>
#include <concepts>
#include <optional>
#include <sstream>
#include <fstream>
#include <utility>
#include <cstdint>
#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <limits>
#include <map>
#include <new>

#include <SFML/Graphics.hpp>

typedef std::int8_t  Int8;
typedef std::int16_t Int16;
typedef std::int32_t Int32;
typedef std::int64_t Int64;

typedef std::uint8_t  UInt8;
typedef std::uint16_t UInt16;
typedef std::uint32_t UInt32;
typedef std::uint64_t UInt64;

typedef std::size_t Size;
typedef std::size_t Offset;

typedef std::string String;

typedef std::filesystem::path Path;

using sf::Vector2f;
using sf::Vector2i;
using sf::Vector2u;

namespace fs = std::filesystem;

namespace utils
{
	static constexpr int game_canvas_with = 1280;
	static constexpr int game_canvas_height = 720;
}

namespace utils
{
	template <typename _Ty = void>
	inline _Ty* malloc(Size size) { return reinterpret_cast<_Ty*>(::operator new(size)); }

	template <typename _Ty>
	inline _Ty* calloc(Size count, Size size) { return reinterpret_cast<_Ty*>(::operator new(size * count)); }

	inline void free(void* ptr) { ::operator delete(ptr); }

	template <typename _Ty, typename... _ArgTys> requires std::constructible_from<_Ty, _ArgTys...>
	constexpr _Ty& construct(_Ty& object, _ArgTys&&... args)
	{
		return *(new(std::addressof(object)) _Ty{std::forward<_ArgTys>(args)...});
	}

	template <std::destructible _Ty>
	constexpr void destroy(_Ty& object) { object.~_Ty(); }

	template <std::copy_constructible _Ty>
	constexpr _Ty& copy_construct(_Ty& dst, const _Ty& src) { return construct(dst, src); }

	template <std::move_constructible _Ty>
	constexpr _Ty& move_construct(_Ty& dst, _Ty&& src) { return construct(dst, std::move(src)); }

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

	inline Int64 systemTime()
	{
		return static_cast<Int64>((std::chrono::system_clock::now().time_since_epoch().count()));
	}
}

namespace utils::bit
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
		return (base >> position) & static_cast<_Ty>(0x1) == static_cast<_Ty>(0x1);
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

