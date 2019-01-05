/** @file fileiterator.hpp
 * @brief Recursively iterates through files in a directory.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_FILEITERATOR_HPP
#define __CS_FILEITERATOR_HPP

#include <memory>

namespace CloudSync {

/**
 * @brief A class that recursively iterates though files in a directory.
 */
class FileIterator {
public:
	/**
	 * @brief Constructs a FileIterator class starting at the specified directory.
	 *
	 * @param baseDir The base directory to start iterating through.
	 *
	 * @exception std::runtime_error Could not open the specified directory.
	 */
	FileIterator(const char* baseDir);

	/**
	 * @brief We have to explicitly define the destructor, otherwise the pImpl unique_ptr has errors determining how to delete the FileIteratorImpl.
	 * Note that we cannot set it equal to default here, as FileIteratorImpl is not defined here.
	 */
	~FileIterator();

	/**
	 * @brief Returns the next entry in the directory, or nullptr if there are no more entries.
	 *
	 * @exception std::runtime_error Failure to stat entry.
	 *
	 * @return The next entry in the directory, or nullptr if there are none.
	 */
	const char* nextEntry();

	/**
	 * @brief Returns the current directory name, or nullptr if there isn't one.
	 */
	const char* currentDirectory() const;

	/**
	 * @brief Skips the current directory.
	 */
	void skipDirectory();

private:
	struct FileIteratorImpl;
	/**
	 * @brief A pointer to the private variables and inner workings of the FileIterator class.
	 */
	std::unique_ptr<FileIteratorImpl> impl;
};

}

#endif
