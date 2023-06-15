#pragma once

#include <cstdint>


using Int8  = std::int8_t;
using Int16 = std::int16_t;
using Int32 = std::int32_t;
using Int64 = std::int64_t;

using Uint8  = std::uint8_t;
using Uint16 = std::uint16_t;
using Uint32 = std::uint32_t;
using Uint64 = std::uint64_t;


enum class RequestFlag
{
	Ready = 0,
	Requested = 1,
	Done = 2
};