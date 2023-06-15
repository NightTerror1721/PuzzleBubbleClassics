#include "logger.h"

#include <string>
#include <chrono>


namespace logger::detail
{
	static inline std::ostream& buffer(Level lv) { return lv >= Level::Warn ? std::cerr : std::cout; }

	static constexpr const char* level_name(Level lv)
	{
		using enum Level;
		switch (lv)
		{
			case Debug: return "DEBUG";
			case Info: return "INFO";
			case Warn: return "WARN";
			case Error: return "ERROR";
			case Fatal: return "FATAL";
			default: return "UNKNOWN";
		}
	}

	static std::string get_time()
	{
		auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		#pragma warning(suppress : 4996)
		std::tm* timemark = std::localtime(&time);
		if (timemark == nullptr)
			return {};

		std::stringstream ss;
		ss << std::put_time(timemark, "%c %z");
		return ss.str();
	}
}



namespace logger
{
	void log(Level log_level, std::string_view msg)
	{
		if (log_level >= SystemLevel)
		{

			detail::buffer(log_level)
				<< "[" << detail::level_name(log_level) << "]"
				<< "[" << detail::get_time() << "]"
				<< ": " << msg << std::endl;
		}
	}
}