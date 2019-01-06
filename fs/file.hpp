/** @file file.hpp
 * @brief Various file functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __FILE_HPP
#define __FILE_HPP

#include <cstdint>

namespace CloudSync::fs {

/**
 * @param Represents the possible types of objects thast can be found at a path.
 */
enum class Type {
	Directory,
	File,
	Symlink,
	Other,
	NotFound,
};

/**
 * @brief Checks if a path is a file, directory, symlink, or doesn't exist.
 *
 * @param path The path.
 *
 * @return File::Type corresponding to what this path points to.
 * File::Type::NotFound if nothing was found at this path.
 *
 * @exception IOException I/O Error
 */
Type getType(const char* path);

/**
 * @brief Checks if a path points to a directory.
 *
 * @param path The path.
 *
 * @return True if the path points to a directory, false if it doesn't exist or does not point to a directory.
 *
 * @exception IOException I/O Error
 */
bool isDirectory(const char* path);

/**
 * @brief Checks if a path points to a file.
 *
 * @param path The path.
 *
 * @return True if the path points to a file, false if it doesn't exist or does not point to a file.
 *
 * @exception IOException I/O Error
 */
bool isFile(const char* path);

/**
 * @brief Checks if a path points to a symlink.
 *
 * @param path The path.
 *
 * @return True if the path points to a symlink, false if it doesn't exist or does not point to a symlink.
 *
 * @exception IOException I/O Error
 */
bool isSymlink(const char* path);

/**
 * @brief Checks if something exists at a given path.
 *
 * @param path The path.
 *
 * @return True if the path points to something, false if not.
 *
 * @exception IOException I/O Error
 */
bool exists(const char* path);

/**
 * @brief Gets the size of a file.
 *
 * @param path The path of the file.
 *
 * @return The size of the file in bytes.
 *
 * @exception NotFoundException A file was not found at this path.
 * @exception IOException The path does not point to a file, or there was an I/O error.
 */
uint64_t size(const char* path);

/**
 * @brief Moves a file/directory/symlink.
 * If the two paths are the same, this function is a no-op.
 *
 * @param src The old path.
 * @param dst The new path.
 *
 * @exception ExistsException The destination already exists.
 * @exception IOException I/O error.
 */
void move(const char* src, const char* dst);

/**
 * @brief Copies a file/directory/symlink.
 * If the two paths are the same, this function is a no-op.
 *
 * @param src The old path.
 * @param dst The new path.
 *
 * @exception ExistsException The destination already exists.
 * @exception IOException I/O error.
 */
void copy(const char* src, const char* dst);

/**
 * @brief Creates a symlink.
 *
 * @param path The path the symlink should be placed at.
 * @param target The path the symlink should point to.
 * This path does not necessarily have to exist.
 *
 * @exception ExistsException The given path already exists.
 * @exception IOException I/O error.
 */
void createSymlink(const char* path, const char* target);

/**
 * @brief Creates a directory.
 * Any parent directories are automatically created.
 *
 * @param path The directory to create.
 *
 * @return True if the directory was created, false if a directory already exists at that path.
 *
 * @exception ExistsException A non-directory exists at the given path.
 * @exception IOException I/O error.
 */
bool createDirectory(const char* path);

}

#endif
