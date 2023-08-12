#pragma once

#include "std.hpp"

enum LogLevel {
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
};

inline void log(std::string message, LogLevel level) {
    std::time_t t = std::time(0);
    std::tm *now = std::localtime(&t);

    std::string level_str;

    switch (level) {
    case LOG_LEVEL_INFO:
        level_str = "[INFO]";
        break;
    case LOG_LEVEL_DEBUG:
        level_str = "[DEBUG]";
        break;
    case LOG_LEVEL_WARN:
        level_str = "[WARN]";
        break;
    case LOG_LEVEL_ERROR:
        level_str = "[ERROR]";
        break;
    case LOG_LEVEL_FATAL:
        level_str = "[FATAL]";
        break;
    }

    std::string hour =
	now->tm_hour > 9 ? std::to_string(now->tm_hour) : "0"
	+ std::to_string(now->tm_hour);
    std::string minute
	= now->tm_min > 9 ? std::to_string(now->tm_min) : "0"
	+ std::to_string(now->tm_min);
    std::string second
	= now->tm_sec > 9 ? std::to_string(now->tm_sec) : "0"
	+ std::to_string(now->tm_sec);

    std::cout
	<< "["
	<< hour
	<< ":"
	<< minute
	<< ":"
	<< second
	<< "] "
	<< level_str
	<< " "
	<< message
	<< std::endl;
}
