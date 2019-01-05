/** @file lnthrow.hpp
 * @brief throws an exception showing line number/file
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <stdexcept>
#include <string>

#define __GET_LNTHROW(_1, _2, _3, NAME, ...) NAME

#define __lnthrow_2(type, msg) throw type(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " + msg)
#define __lnthrow_3(type, msg, reason) throw type(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " + msg + "\n" + reason.what());

#define lnthrow(...) __GET_LNTHROW(__VA_ARGS__, __lnthrow_3, __lnthrow_2, __lnthrow_2, __lnthrow_2)(__VA_ARGS__)
