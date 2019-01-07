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
 * @exception NotFoundException A file/folder does not exist at the given path.
 * @exception IOException I/O error.
 */
std::vector<unsigned char> serializePerms(const char* path);

/**
 * @param Converts a serialized set of bytes back into their corresponding file permissions and applies them to the given path.
 *
 * @param path The path to apply permissions to.
 * @param perms A byte vector containing the permissions.
 *
 * @exception NotFoundException A file/folder does not exist at the given path.
 * @exception IOException I/O error.
 */
void deserializePerms(const char* path, const std::vector<unsigned char>& perms);

/**
 * @param Converts a serialized set of bytes back into their corresponding file permissions and applies them to the given path.
 *
 * @param path The path to apply permissions to.
 * @param perms A pointer to bytes containing the permissions.
 * @param permsLen The length of the aforementioned bytes.
 *
 * @exception NotFoundException A file/folder does not exist at the given path.
 * @exception IOException I/O error.
 */
void deserializePerms(const char* path, void* perms, size_t permsLen);

}

#endif
