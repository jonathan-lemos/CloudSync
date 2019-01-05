/** @file fileperms.hpp
 * @brief File permission functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __FILEPERMS_HPP
#define __FILEPERMS_HPP

#include <vector>

namespace CloudSync::fs {

/**
 * @param Converts a file's permissions into a series of bytes.
 * The format of these bytes is dependent on operating system.
 * This series of bytes is guaranteed to be able to be converted into the file's original permissions with deserializeBytes()
 *
 * @exception CsError A file/folder does not exist at the given path, or there was an I/O error.
 */
std::vector<unsigned char> serializePerms(const char* path);
void deserializePerms(const char* path, const std::vector<unsigned char>& perms);
void deserializePerms(const char* path, void* perms, size_t permsLen);

}

#endif
