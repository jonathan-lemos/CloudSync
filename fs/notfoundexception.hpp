/** @file notfoundexception.hpp
 * @brief Thrown when a file is not found.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __NOTFOUNDEXCEPTION_HPP
#define __NOTFOUNDEXCEPTION_HPP

#include <stdexcept>

namespace CloudSync::fs {

class NotFoundException : public std::runtime_error {
	NotFoundException(std::string msg) : std::runtime_error(msg) {}
};

}

#endif
