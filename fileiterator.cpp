/** @file fileiterator.cpp
 * @brief Recursively iterates through files in a directory.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifdef __linux__
#include "os/linux/fileiterator.cpp"
#else
#error "Support for this operating system is not included yet."
#endif
