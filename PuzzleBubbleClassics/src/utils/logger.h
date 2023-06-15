#pragma once

#include <string>
#include <format>

#include "io.h"

namespace logger
{
	enum class Level
	{
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};

	inline constexpr Level SystemLevel = Level::Warn;
}



namespace logger
{
	void log(Level level, std::string_view msg);


	template <Level _Level>
	struct Buffer
	{
		static constexpr Level lv = _Level;

		constexpr Buffer() = default;
		constexpr Buffer(const Buffer&) = delete;
		constexpr Buffer& operator= (const Buffer&) = delete;
		constexpr ~Buffer() = default;

		inline void operator() (std::string_view msg) const { if constexpr (SystemLevel <= lv) log(lv, msg); }

		template <typename... _ArgTys>
		inline void operator() (std::string_view fmt, _ArgTys&&... args) const
		{
			if constexpr (SystemLevel <= lv)
			{
				log(lv, std::vformat(fmt, std::make_format_args(std::forward<_ArgTys>(args)...)));
			}
		}

		template <typename _Ty> requires requires(std::stringstream& ss, const _Ty& v)
		{
			{ ss << v };
		}
		inline const Buffer& operator<< (const _Ty& right) const
		{
			if constexpr (SystemLevel <= lv)
			{
				std::stringstream ss;
				ss << right;
				return log(lv, ss.str()), * this;
			}
		}
	};

	inline constexpr const Buffer<Level::Debug> debug;
	inline constexpr const Buffer<Level::Info> info;
	inline constexpr const Buffer<Level::Warn> warn;
	inline constexpr const Buffer<Level::Error> error;
	inline constexpr const Buffer<Level::Fatal> fatal;
}
