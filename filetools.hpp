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
#include <memory>
#include <optional>

namespace CloudSync::File {

/**
 * @brief Checks if a file exists.
 *
 * @param path The path to check.
 *
 * @return True if a file exists at this path, false if not.
 * Returns false if a directory exists at the path.
 * @see std::strerror()
 */
bool fileExists(const char* path);

/**
 * @brief Checks if a directory exists.
 *
 * @param path The path to check.
 *
 * @return True if a directory exists at this path, false if not.
 * Returns false if a file exists at the path.
 * @see std::strerror()
 */
bool directoryExists(const char* path);

/**
 * @brief Returns the size of a file.
 *
 * @param path Path to a file.
 *
 * @return The size of the file, or std::nullopt if there was an error.
 * @see std::strerror()
 */
std::optional<uint64_t> fileSize(const char* path);

/**
 * @brief Class that holds a temporary file.
 */
class TmpFile {
public:
	/**
	 * @brief Constructs a temporary file.
	 * The name of the temporary file can be retrieved with TmpFile::name().
	 * The file stream of this file can be retrieved with TmpFile::fs().
	 *
	 * It is constructed in the default temporary directory for this system.
	 *
	 * @exception std::runtime_error There was an error creating the file.
	 */
	TmpFile();
	/**
	 * @brief Destructs a temporary file.
	 * When this fires the file is deleted.
	 */
	~TmpFile();
	/**
	 * @brief Returns the internal fstream of this TmpFile.
	 * If the file is edited through outside sources, it must be refreshed with TmpFile::refresh().
	 */
	std::fstream& fs();
	/**
	 * @brief Returns the filename of this TmpFile.
	 */
	const char* name();
	/**
	 * @brief Re-initializes the internal fstream.
	 * This is necessary if the internal fstream is closed or its file is edited through outside sources.
	 */
	void refresh();
private:
	struct TmpFileImpl;
	std::unique_ptr<TmpFileImpl> impl;
};

}


#endif
