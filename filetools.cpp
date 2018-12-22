/** @file filetools.cpp
 * @brief Functions for dealing with files.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifdef __linux__
#include "os/linux/filetools.cpp"
#else
#error "Support for this operating system is not included yet."
#endif
