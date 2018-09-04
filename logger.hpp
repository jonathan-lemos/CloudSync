/** @file logger.hpp
 * @brief Thread-safe logging class.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_LOGGER_HPP
#define __CS_LOGGER_HPP

#include <iostream>
#include <mutex>

namespace CloudSync{

#define LOG(level) Logger(level, __FILE__, __LINE__, __func__)

enum LogLevel{
	LEVEL_NONE    = 0,
	LEVEL_FATAL   = 1,
	LEVEL_ERROR   = 2,
	LEVEL_WARNING = 3,
	LEVEL_DEBUG   = 4,
	LEVEL_INFO    = 5
};

class Logger{
public:
	Logger(LogLevel ll, const char* file, int line, const char* func);
	static void setLevel(LogLevel ll);
	static void setStream(std::ostream& os);
	virtual ~Logger();

	template<typename T>
	Logger& operator<<(const T& val);
private:
	LogLevel currentLevel;
	static LogLevel reportingLevel;
	static std::ostream* os;
	static std::mutex mutex;
};

}

#endif
