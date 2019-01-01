/** @file cserror.hpp
 * @brief Exception with line number/file.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdexcept>

class __CsError : public std::runtime_error {
public:
	__CsError(const std::string& msg, const char* file, int line) :
		std::runtime_error(std::string(file) + ":" + std::to_string(line) + ": " + msg) {}
};

#define CsError(msg) __CsError(msg, __FILE__, __LINE__)
