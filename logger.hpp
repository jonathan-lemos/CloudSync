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
#include <sstream>
#include <mutex>

namespace CloudSync{

/**
 * @brief Use this macro to instantiate the Logger class.
 * @see Logger
 */
#define LOG(level) Logger(level, __FILE__, __LINE__, __func__)

/**
 * @brief The severity of the current message, LEVEL_FATAL being the most severe and LEVEL_INFO being the least.
 */
enum LogLevel{
	LEVEL_NONE    = 0,
	LEVEL_FATAL   = 1,
	LEVEL_ERROR   = 2,
	LEVEL_WARNING = 3,
	LEVEL_DEBUG   = 4,
	LEVEL_INFO    = 5
};

/**
 * @brief CloudSync's logging class.
 * Use it like you would std::cout or any other standard stream. For example:
 * ```C
 * LOG(LEVEL_WARNING) << "Error code:" << integer1;
 * ```
 * A newline is automatically placed after the message.
 * Unfortunately, std::endl and other stream manipulators are not supported.
 * This class and all of its methods are thread-safe.
 */
class Logger{
public:
	/**
	 * @brief Do not instantiate the Logger class directly.
	 * Use the LOG() macro instead.
	 */
	Logger(LogLevel ll, const char* file, int line, const char* func);

	/**
	 * @brief Sets the current logging severity.
	 * Any messages over this level will not be displayed.
	 *
	 * @param ll The level to set.
	 */
	static void setLevel(LogLevel ll);

	/**
	 * @brief Sets the current output stream.
	 * By default this is set to std::cerr.
	 *
	 * @param os The output stream to set.
	 */
	static void setStream(std::ostream& os);

	/**
	 * @brief Outputs the message and puts a newline at the end of it.
	 */
	virtual ~Logger();

	/**
	 * @brief The '<<' operator is used just like std::cout's '<<' operator.
	 */
	template<typename T>
	Logger& operator<<(const T& val){
		// the definition has to be in the header file because C++'s linker is bogus and can't find the implementation for a template
		std::unique_lock<std::mutex> lock(loggerStateMutex);
		if (currentLevel <= reportingLevel){
			ss << val;
		}
		return *this;
	}

private:
	/**
	 * @brief Buffers the message so it can be outputted atomically.
	 * This is needed for thread-safety.
	 */
	std::stringstream ss;

	/**
	 * @brief The level of the current message.
	 */
	LogLevel currentLevel;

	/**
	 * @brief The global reporting level.
	 * Any messages above this level will not be displayed.
	 */
	static LogLevel reportingLevel;

	/**
	 * @brief The stream to output to.
	 * This is set to std::cerr by default.
	 */
	static std::ostream* os;

	/**
	 * @brief Locks the static variables of the Logger
	 */
	static std::mutex loggerStateMutex;
};

}

#endif
