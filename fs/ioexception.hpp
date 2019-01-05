/** @file ioexception.hpp
 * @brief Thrown when an I/O error took place.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __IOEXCEPTION_HPP
#define __IOEXCEPTION_HPP

#include <stdexcept>

namespace CloudSync::fs {

class IOException : public std::runtime_error {
	IOException(std::string msg) : std::runtime_error(msg) {}
};

}

#endif
