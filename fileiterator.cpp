/** @file fileiterator.cpp
 * @brief Recursively iterates through files in a directory.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "fileiterator.hpp"
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;

namespace CloudSync{

struct FileIterator::FileIteratorImpl {
	fs::path baseDir;
	fs::recursive_directory_iterator iter;
	std::mutex m;
	fs::path currentPath;
};

FileIterator::FileIterator(const char* baseDir): impl(std::make_unique<FileIteratorImpl>()) {
	this->impl->baseDir = baseDir;
	this->impl->iter = fs::recursive_directory_iterator(this->impl->baseDir);
}

FileIterator::~FileIterator() = default;

const char* FileIterator::nextEntry() {
	auto end = fs::recursive_directory_iterator();
	if (this->impl->iter == end) {
		return nullptr;
	}
	const char* s = this->impl->iter->path().c_str();
	++this->impl->iter;
	return s;
}

const char* FileIterator::currentDirectory() const {
	return this->impl->iter->path().c_str();
}

void FileIterator::skipDirectory() {
	this->impl->iter.pop();
}

}
