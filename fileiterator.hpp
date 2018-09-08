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

namespace CloudSync{

struct FileIteratorImpl;

/**
 * A class that recursively iterates though files in a directory.
 */
class FileIterator{
public:
	/**
	 * Constructs a FileIterator class starting at the specified directory.
	 *
	 * @param baseDir The base directory to start iterating through.
	 *
	 * @exception std::runtime_error Could not open the specified directory (see what() for details)
	 */
	FileIterator(const char* baseDir);

	/**
	 * Destructs a FileIterator
	 */
	~FileIterator();

	/**
	 * Returns the next entry in the directory, or nullptr if there are no more entries.
	 *
	 * @exception std::runtime_error Failure to stat entry.
	 *
	 * @return The next entry in the directory, or nullptr if there are none.
	 */
	const char* nextEntry();
private:
	std::unique_ptr<FileIteratorImpl> impl;
};

}

#endif
