/** @file filetools.hpp
 * @brief Functions for dealing with files.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_FILETOOLS_HPP
#define __CS_FILETOOLS_HPP

#include "attribute.hpp"
#include <cstdint>
#include <fstream>
#include <optional>

namespace CloudSync::File {

/**
 * Checks if a file exists.
 *
 * @param path The path to check.
 *
 * @return True if a file exists at this path, false if not.
 * Returns false if a directory exists at the path.
 * @see std::strerror()
 */
bool fileExists(const char* path);

/**
 * Checks if a directory exists.
 *
 * @param path The path to check.
 *
 * @return True if a directory exists at this path, false if not.
 * Returns false if a file exists at the path.
 * @see std::strerror()
 */
bool directoryExists(const char* path);

/**
 * Returns the size of a file.
 *
 * @param path Path to a file.
 *
 * @return The size of the file, or std::nullopt if there was an error.
 * @see std::strerror()
 */
std::optional<uint64_t> fileSize(const char* path);

/**
 * Opens a temporary file and returns its name and fstream.
 *
 * @return The filename and fstream of the temp file, or std::nullopt if there was an error.
 * @see std::strerror()
 */
std::optional<std::pair<std::string, std::fstream>> openTmp();

}


#endif
