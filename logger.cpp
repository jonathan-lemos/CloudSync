/** @file logger.cpp
 * @brief Thread-safe logging class.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "logger.hpp"
#include "color.hpp"
#include "io_mutex.hpp"

namespace CloudSync{

std::ostream* Logger::os = &(std::cerr);

Logger::Logger(LogLevel ll, const char* file, int line, const char* func){
	currentLevel = ll;
	if (currentLevel != LEVEL_NONE && currentLevel <= reportingLevel){
		switch (currentLevel){
			case LEVEL_FATAL:
				ss << "[" << Color::COLOR_MAGENTA << "FATAL" << Color::COLOR_NORMAL << "]";
				break;
			case LEVEL_ERROR:
				ss << "[" << Color::COLOR_RED << "ERROR" << Color::COLOR_NORMAL << "]";
				break;
			case LEVEL_WARNING:
				ss << "[" << Color::COLOR_YELLOW << "WARN " << Color::COLOR_NORMAL << "]";
				break;
			case LEVEL_DEBUG:
				ss << Color::COLOR_CYAN << "DEBUG" << Color::COLOR_NORMAL << "]";
				break;
			case LEVEL_INFO:
				ss << Color::COLOR_GREEN << "INFO" << Color::COLOR_NORMAL << "]";
				break;
			default:
				;
		}
		ss << "(" << file << ":" << line << ":" << func << ")";
	}
}

void Logger::setLevel(LogLevel ll){
	reportingLevel = ll;
}

void Logger::setStream(std::ostream& os){
	Logger::os = &os;
}

template<typename T>
Logger& Logger::operator<<(const T& val){
	if (currentLevel <= reportingLevel){
		ss << val;
	}
}

Logger::~Logger(){
	if (currentLevel <= reportingLevel){
		ss << std::endl;

		io_mutex.lock();
		*os << ss.str();
		io_mutex.unlock();
	}
}

}
