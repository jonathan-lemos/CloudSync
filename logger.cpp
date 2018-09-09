/** @file logger.cpp
 * @brief Thread-safe logging class.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "logger.hpp"
#include "io_mutex.hpp"
#include "terminal.hpp"
#include <mutex>

namespace CloudSync{

std::ostream* Logger::os = &(std::cerr);
std::mutex loggerStateMutex;

Logger::Logger(LogLevel ll, const char* file, int line, const char* func){
	std::unique_lock lock(loggerStateMutex);
	currentLevel = ll;
	if (currentLevel != LEVEL_NONE && currentLevel <= reportingLevel){
		switch (currentLevel){
		case LEVEL_FATAL:
			ss << "[" << Terminal::Color(Terminal::MAGENTA) << "FATAL" << Terminal::Color(Terminal::NORMAL) << "]";
			break;
		case LEVEL_ERROR:
			ss << "[" << Terminal::Color(Terminal::RED) << "ERROR" << Terminal::Color(Terminal::NORMAL) << "]";
			break;
		case LEVEL_WARNING:
			ss << "[" << Terminal::Color(Terminal::YELLOW) << "WARN " << Terminal::Color(Terminal::NORMAL) << "]";
			break;
		case LEVEL_DEBUG:
			ss << Terminal::Color(Terminal::CYAN) << "DEBUG" << Terminal::Color(Terminal::NORMAL) << "]";
			break;
		case LEVEL_INFO:
			ss << Terminal::Color(Terminal::GREEN) << "INFO" << Terminal::Color(Terminal::NORMAL) << "]";
			break;
		default:
			;
		}
		ss << "(" << file << ":" << line << ":" << func << ")";
	}
}

void Logger::setLevel(LogLevel ll){
	std::unique_lock lock(loggerStateMutex);
	reportingLevel = ll;
}

void Logger::setStream(std::ostream& os){
	std::unique_lock lock(loggerStateMutex);
	Logger::os = &os;
}

template<typename T>
Logger& Logger::operator<<(const T& val){
	std::unique_lock lock(loggerStateMutex);
	if (currentLevel <= reportingLevel){
		ss << val;
	}
}

Logger::~Logger(){
	std::unique_lock lock(loggerStateMutex);
	if (currentLevel <= reportingLevel){
		ss << std::endl;

		IO::io_mutex.lock();
		*os << ss.str();
		IO::io_mutex.unlock();
	}
}

}
