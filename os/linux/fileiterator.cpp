/** @file fileiterator.cpp
 * @brief Recursively iterates through files in a directory.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../../fileiterator.hpp"
#include "../../cserror.hpp"
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
	DirEntry(const DirEntry& other) = delete;
	DirEntry& operator=(const DirEntry& other) = delete;
	DirEntry(DirEntry&& other);
	DirEntry& operator=(DirEntry&& other);
	~DirEntry();               /**< Closes dp and destructs DirEntry class. */

	DIR* dp;                   /**< The DIR* corresponding to the directory. */
	struct dirent* dnt;        /**< An entry within the directory. */
	std::string name;          /**< The name of the directory. */
	std::string current;
};

DirEntry::DirEntry(const char* dir): dnt(nullptr), name(dir) {
	this->dp = opendir(dir);
	if (!(this->dp)){
		CSTHROW(std::string("Failed to open directory \"") + dir + "\" (" + std::strerror(errno) + ")");
	}
}

DirEntry::DirEntry(DirEntry&& other) {
	this->dp = other.dp;
	other.dp = nullptr;
	this->dnt = other.dnt;
	other.dnt = nullptr;
	this->name = other.name;
	other.name = "";
	this->current = other.current;
	other.current = "";
}

DirEntry& DirEntry::operator=(DirEntry&& other) {
	this->dp = other.dp;
	other.dp = nullptr;
	this->dnt = other.dnt;
	other.dnt = nullptr;
	this->name = other.name;
	other.name = "";
	this->current = other.current;
	other.current = "";
	return *this;
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
	std::recursive_mutex m;
};

FileIterator::FileIterator(const char* baseDir): impl(std::make_unique<FileIteratorImpl>()) {
	impl->dirStack.push(DirEntry(baseDir));
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
	struct stat st;
	// lock mutex to prevent multiple threads from accessing the same entry
	// std::unique_lock<std::recursive_mutex> lock(impl->m);

	// If we don't have a current directory.
	if (impl->dirStack.empty()) {
		return nullptr;
	}

	// Get the current directory.
	DirEntry& entry = impl->dirStack.top();
	// Then get the next entry in that directory.
	entry.dnt = readdir(entry.dp);
	// If we are out of entries in this directory.
	if (!entry.dnt) {
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
	entry.current = make_path(entry.name.c_str(), entry.dnt->d_name);
	if (lstat(entry.current.c_str(), &st) != 0){
		CSTHROW(std::string("Failed to stat \"") + entry.current.c_str() + "\" (" + std::strerror(errno) + ")");
	}

	// If this entry is a directory.
	if (S_ISDIR(st.st_mode)){
		// Push it onto the stack and get its first entry.
		impl->dirStack.push(DirEntry(entry.current.c_str()));
		return nextEntry();
	}
	// Otherwise return this entry.
	return entry.current.c_str();
}

const char* FileIterator::currentDirectory() {
	if (this->impl->dirStack.empty()) {
		return nullptr;
	}
	return this->impl->dirStack.top().name.c_str();
}

void FileIterator::skipDirectory() {
	if (this->impl->dirStack.empty()) {
		CSTHROW("Cannot skipDirectory() when stack is empty");
	}
	this->impl->dirStack.pop();
}

}
