/** @file fileiterator.cpp
 * @brief Recursively iterates through files in a directory.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../../fileiterator.hpp"
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <mutex>
#include <stack>
#include <stdexcept>
#include <sys/stat.h>

namespace CloudSync{

/**
 * A directory entry.
 */
struct DirEntry {
	DirEntry(const char* dir); /**< Constructs a DirEntry class corresponding to the specified directory. */
	~DirEntry();               /**< Closes dp and destructs DirEntry class. */
	DIR* dp;                   /**< The DIR* corresponding to the directory. */
	struct dirent* dnt;        /**< An entry within the directory. */
	std::string name;          /**< The name of the directory. */
};

DirEntry::DirEntry(const char* dir): dnt(nullptr), name(dir) {
	this->dp = opendir(dir);
	if (!(this->dp)){
		throw std::runtime_error(std::string("Failed to open directory \"") + dir + "\" (" + std::strerror(errno) + ")");
	}
}

DirEntry::~DirEntry() {
	if (this->dp){
		closedir(this->dp);
	}
}

struct FileIterator::FileIteratorImpl {
	/**
	 * @brief The current list of directories as a stack.
	 * This is needed to traverse back up when we run out of entries in the current subdirectory.
	 */
	std::stack<DirEntry> dirStack;
	/**
	 * @brief The mutex allows this class to work with multiple threads.
	 */
	std::mutex m;
};

FileIterator::FileIterator(const char* baseDir): impl(std::make_unique<FileIteratorImpl>()) {
	impl->dirStack.push(baseDir);
}

// Explicit destructor to allow unique_ptr destruction to work properly.
FileIterator::~FileIterator() = default;

static std::string make_path(const char* dir, const char* d_name) {
	std::string ret = dir;
	if (ret[ret.length() - 1] != '/'){
		ret += '/';
	}
	ret += d_name;
	return ret;
}

const char* FileIterator::nextEntry() {
	std::string ret;
	struct stat st;
	// lock mutex to prevent multiple threads from accessing the same entry
	std::unique_lock<std::mutex> lock(impl->m);

	// If we don't have a current directory.
	if (impl->dirStack.empty()){
		return nullptr;
	}

	// Get the current directory.
	DirEntry& entry = impl->dirStack.top();
	// Then get the next entry in that directory.
	entry.dnt = readdir(entry.dp);
	// If we are out of entries in this directory.
	if (!entry.dnt){
		// Go back up a level and return that entry.
		impl->dirStack.pop();
		return nextEntry();
	}

	// If "." or ".." are pulled.
	if (!strcmp(entry.dnt->d_name, ".") ||
			!strcmp(entry.dnt->d_name, "..")){
		// Skip this entry.
		return nextEntry();
	}

	// Get the full path of the file from the directory and d_name.
	ret = make_path(entry.name.c_str(), entry.dnt->d_name);
	if (lstat(ret.c_str(), &st) != 0){
		throw std::runtime_error(strerror(errno));
	}

	// If this entry is a directory.
	if (S_ISDIR(st.st_mode)){
		// Push it onto the stack and get its first entry.
		impl->dirStack.push(ret.c_str());
		return nextEntry();
	}
	// Otherwise return this entry.
	return ret.c_str();
}

const char* FileIterator::currentDirectory() {
	return this->impl->dirStack.top().name.c_str();
}

void FileIterator::skipDirectory() {
	this->impl->dirStack.pop();
}

}
