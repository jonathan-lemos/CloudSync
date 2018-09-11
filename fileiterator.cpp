/** @file fileiterator.cpp
 * @brief Recursively iterates through files in a directory.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "fileiterator.hpp"
#include <stack>
#include <mutex>
#include <dirent.h>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>

namespace CloudSync{

/**
 * A directory entry.
 */
class DirEntry{
public:
	DirEntry(const char* dir); /**< Constructs a DirEntry class corresponding to the specified directory. */
	~DirEntry();                /**< Closes dp and destructs DirEntry class. */
	DIR* dp;                   /**< The DIR* corresponding to the directory. */
	struct dirent* dnt;        /**< An entry within the directory. */
	std::string name;          /**< The name of the directory. */
};

DirEntry::DirEntry(const char* dir): dnt(nullptr), name(dir){
	dp = opendir(dir);
	if (!dp){
		throw std::runtime_error(std::strerror(errno));
	}
}

DirEntry::~DirEntry(){
	if (dp){
		closedir(dp);
	}
}

struct FileIterator::FileIteratorImpl{
	std::stack<DirEntry> dirStack;
	std::mutex m;
};

FileIterator::FileIterator(const char* baseDir): impl(std::make_unique<FileIteratorImpl>()){
	impl->dirStack.push(baseDir);
}

std::string make_path(const char* dir, const char* d_name){
	std::string ret = dir;
	if (ret[ret.length() - 1] != '/'){
		ret += '/';
	}
	ret += d_name;
	return ret;
}

const char* FileIterator::nextEntry(){
	std::string ret;
	struct stat st;
	std::unique_lock<std::mutex> lock(impl->m);

	if (impl->dirStack.empty()){
		return nullptr;
	}

	DirEntry& entry = impl->dirStack.top();
	entry.dnt = readdir(entry.dp);
	if (!entry.dnt){
		impl->dirStack.pop();
		return nextEntry();
	}

	if (!strcmp(entry.dnt->d_name, ".") ||
			!strcmp(entry.dnt->d_name, "..")){
		return nextEntry();
	}

	ret = make_path(entry.name.c_str(), entry.dnt->d_name);
	if (lstat(ret.c_str(), &st) != 0){
		throw std::runtime_error(strerror(errno));
	}

	if (S_ISDIR(st.st_mode)){
		impl->dirStack.push(ret.c_str());
		return nextEntry();
	}
	return ret.c_str();
}

}
