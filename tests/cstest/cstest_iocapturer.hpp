/** @file cstest_iocapturer.hpp
 * @brief CloudSync testing framework I/O Capturer.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../../attribute.hpp"
#include <memory>
#include <string>
#include <cstdarg>

namespace CloudSync{
namespace Testing{

struct IOCapturerImpl;

class IOCapturer{
public:
	/**
	 * @brief Begins capturing std(out/err/in).
	 * This output is not echoed to the screen while it is being captured.
	 * Capture stops when the IOCapturer function is destructed.
	 */
	IOCapturer();

	/**
	 * @brief Stops capturing std(out/err/in).
	 */
	~IOCapturer();

	/**
	 * @brief Returns all of what was sent to stdout since the last call to getStdout()
	 *
	 * @return stdout's input, including all newlines.
	 */
	std::string getStdout();

	/**
	 * @brief Returns all of what was sent to stderr since the last call to getStderr()
	 *
	 * @return stderr's input, including all newlines.
	 */
	std::string getStderr();

	/**
	 * @brief Sends a line to stdin.
	 *
	 * @param line The line you want to send.
	 * If this does not end with a '\n', one will be appended automatically.
	 */
	void sendToStdin(const char* line);

	/**
	 * @brief Prints a line to the screen.
	 * This output of this function is not captured through getStdout() or getStderr()
	 * This function uses printf(3) syntax.
	 *
	 * @param format A printf format string.
	 * @param ... The corresponding printf varargs, if any.
	 *
	 * @return The number of characters successfully written.
	 */
	int CS_PRINTF_LIKE(1) printToScreen(const char* format, ...);

private:
	static int instanceCount;
	std::unique_ptr<IOCapturerImpl> impl;
};

}
}
