/** @file cstest_iocapturer.hpp
 * @brief CloudSync testing framework I/O capturer.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cstest_iocapturer.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <iostream>

#define P_READ  (0)
#define P_WRITE (1)

namespace CloudSync{
namespace Testing{

int IOCapturer::instanceCount = 0;

struct IOCapturerImpl{
	int stdoutOld = 0;
	int stderrOld = 0;
	int stdinOld  = 0;
	int stdoutPipe[2] = {0, 0};
	int stderrPipe[2] = {0, 0};
	int stdinPipe [2] = {0, 0};
};

IOCapturer::IOCapturer(): impl(std::make_unique<IOCapturerImpl>()){
	if (instanceCount != 0){
		throw std::logic_error("Only one instance of IOCapturer can be active at a time");
	}
	instanceCount++;

	if (pipe(impl->stdoutPipe) != 0){
		throw std::runtime_error("Failed to create stdout pipe (" + std::string(std::strerror(errno)) + ")");
	}

	if (pipe(impl->stderrPipe) != 0){
		throw std::runtime_error("Failed to create stderr pipe (" + std::string(std::strerror(errno)) + ")");
	}

	if (pipe(impl->stdinPipe) != 0){
		throw std::runtime_error("Failed to create stdin pipe (" + std::string(std::strerror(errno)) + ")");
	}

	impl->stdoutOld = dup(STDOUT_FILENO);
	impl->stderrOld = dup(STDOUT_FILENO);
	impl->stdinOld  = dup(STDIN_FILENO);

	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	dup2(impl->stdoutPipe[P_WRITE], STDOUT_FILENO);
	dup2(impl->stderrPipe[P_WRITE], STDERR_FILENO);
	dup2(impl->stdinPipe[P_READ], STDIN_FILENO);
}

std::string IOCapturer::getStdout(){
	char buf[1024];
	std::string s = "";
	ssize_t ss;

	fcntl(impl->stdoutPipe[P_READ], F_SETFL, O_NONBLOCK);
	while ((ss = read(impl->stdoutPipe[P_READ], buf, sizeof(buf) - 1)) > 0){
		buf[ss] = '\0';
		s += buf;
	}
	return s;
}

std::string IOCapturer::getStderr(){
	char buf[1024];
	std::string s = "";
	ssize_t ss;

	fcntl(impl->stderrPipe[P_READ], F_SETFL, O_NONBLOCK);
	while ((ss = read(impl->stderrPipe[P_READ], buf, sizeof(buf) - 1)) > 0){
		buf[ss] = '\0';
		s += buf;
	}
	return s;
}

std::string IOCapturer::getLastLine(std::string input){
	size_t pos = input.find_last_of('\n');
	if (pos == std::string::npos){
		return "";
	}
	if (pos == input.size() - 1){
		input.resize(input.size() - 1);
		return getLastLine(input);
	}
	return std::string(input.c_str() + pos + 1);
}

void IOCapturer::sendToStdin(const char* line){
	const char newline = '\n';

	fcntl(impl->stdinPipe[P_WRITE], F_SETFL, O_NONBLOCK);
	write(impl->stdinPipe[P_WRITE], line, std::strlen(line));
	if (line[std::strlen(line) - 1] != '\n'){
		write(impl->stdinPipe[P_WRITE], &newline, 1);
	}
}

int IOCapturer::printToScreen(const char* format, ...){
	char* buf = NULL;
	va_list ap;
	int len;

	va_start(ap, format);

	len = std::vsnprintf(buf, 0, format, ap);
	if (len < 0){
		va_end(ap);
		throw std::logic_error("Invalid printf expression");
	}

	buf = new char[len];
	if (!buf){
		va_end(ap);
		throw std::runtime_error("Out of memory");
	}

	if (std::vsnprintf(buf, len, format, ap) != len){
		va_end(ap);
		delete[] buf;
		throw std::runtime_error("vsnprintf() write error");
	}

	if (write(impl->stdoutOld, buf, len) != 0){
		va_end(ap);
		delete[] buf;
		throw std::runtime_error("write() error");
	}

	delete[] buf;
	return len;
}

IOCapturer::~IOCapturer(){
	instanceCount--;
	dup2(impl->stdoutOld, STDOUT_FILENO);
	dup2(impl->stderrOld, STDERR_FILENO);
	dup2(impl->stdinOld,  STDIN_FILENO);
}

}
}
