/** @file treewalker.hpp
 * @brief Recursively iterates through files in a directory.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_TREEWALKER_HPP
#define __CS_TREEWALKER_HPP

#include <memory>

namespace CloudSync::fs {

/**
 * @brief A class that recursively iterates though files in a directory.
 */
class TreeWalker {
public:
	/**
	 * @brief Constructs a TreeWalker class starting at the specified directory.
	 *
	 * @param baseDir The base directory to start iterating through.
	 *
	 * @exception NotFoundException A directory does not exist at this path.
	 * @exception IOException I/O error.
	 */
	TreeWalker(const char* baseDir);

	/**
	 * @brief We have to explicitly define the destructor, otherwise the pImpl unique_ptr has errors determining how to delete the TreeWalkerImpl.
	 * Note that we cannot set it equal to default here, as TreeWalkerImpl is not defined here.
	 */
	~TreeWalker();

	/**
	 * @brief Returns the next entry in the directory, or nullptr if there are no more entries.
	 *
	 * @return The next entry in the directory, or nullptr if there are none.
	 *
	 * @exception IOException I/O error.
	 */
	const char* nextEntry();

	/**
	 * @brief Returns the current directory name, or nullptr if there isn't one.
	 */
	const char* currentDirectory() const noexcept;

	/**
	 * @brief Skips the current directory.
	 */
	void skipDirectory() noexcept;

private:
	struct TreeWalkerImpl;
	/**
	 * @brief A pointer to the private variables and inner workings of the TreeWalker class.
	 */
	std::unique_ptr<TreeWalkerImpl> impl;
};

}

#endif
