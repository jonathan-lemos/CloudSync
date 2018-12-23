/** @file terminal.hpp
 * @brief Terminal modifying functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifdef __linux__
#include "os/linux/terminal.cpp"
#else
#error "Support for this operating system is not included yet."
#endif
