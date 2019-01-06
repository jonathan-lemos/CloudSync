/** @file treewalker.cpp
 * @brief Recursively iterates through files in a directory.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "treewalker.hpp"
#include "file.hpp"
#include "ioexception.hpp"
#include "notfoundexception.hpp"
#include "../lnthrow.hpp"
#include <filesystem>
#include <mutex>

namespace CloudSync::fs {

struct TreeWalker::TreeWalkerImpl {
	std::filesystem::path baseDir;
	std::filesystem::recursive_directory_iterator iter;
	std::mutex m;
	std::filesystem::path currentPath;
};

TreeWalker::TreeWalker(const char* baseDir): impl(std::make_unique<TreeWalkerImpl>()) {
	if (!isDirectory(baseDir)) {
		lnthrow(NotFoundException, "\"" + std::string(baseDir) + "\" does not point to a directory");
	}

	this->impl->baseDir = baseDir;
	try {
		this->impl->iter = std::filesystem::recursive_directory_iterator(this->impl->baseDir);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, "Failed to create recursive_directory_iterator.", e);
	}
}

TreeWalker::~TreeWalker() = default;

const char* TreeWalker::nextEntry() {
	std::unique_lock<std::mutex>(this->impl->m);
	auto end = std::filesystem::recursive_directory_iterator();
	if (this->impl->iter == end) {
		return nullptr;
	}
	const char* s = this->impl->iter->path().c_str();
	++this->impl->iter;
	return s;
}

const char* TreeWalker::currentDirectory() const noexcept {
	return this->impl->iter->path().c_str();
}

void TreeWalker::skipDirectory() noexcept {
	std::unique_lock<std::mutex>(this->impl->m);
	this->impl->iter.pop();
}

}
