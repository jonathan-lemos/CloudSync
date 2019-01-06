/** @file existsexception.hpp
 * @brief Thrown when a path exists and it shouldn't
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __EXISTSEXCEPTION_HPP
#define __EXISTSEXCEPTION_HPP

#include <stdexcept>

namespace CloudSync::fs {

class ExistsException : public std::runtime_error {
public:
	ExistsException(std::string msg) : std::runtime_error(msg) {}
};

}

#endif
