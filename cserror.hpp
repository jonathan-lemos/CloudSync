/** @file cserror.hpp
 * @brief Exception with line number/file.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdexcept>

namespace CloudSync {

class CsError : public std::runtime_error {
public:
	CsError(const std::string& msg, const char* file, int line) :
		std::runtime_error(std::string(file) + ":" + std::to_string(line) + ": " + msg) {}
};

}

#define CSTHROW(msg) throw CloudSync::CsError(msg, __FILE__, __LINE__)
