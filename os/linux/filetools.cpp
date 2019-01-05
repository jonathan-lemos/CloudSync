/** @file os/linux/filetools.cpp
 * @brief Functions for dealing with files.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../../filetools.hpp"
#include "../../cserror.hpp"
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

namespace CloudSync::File {

bool fileExists(const char* path) {
	struct stat st;

	if (stat(path, &st) != 0) {
		return false;
	}
	return S_ISREG(st.st_mode);
}

bool directoryExists(const char* path) {
	struct stat st;

	if (stat(path, &st) != 0) {
		return false;
	}
	return S_ISDIR(st.st_mode);
}

void rename(const char* fnOld, const char* fnNew) {
	if (std::rename(fnOld, fnNew) == 0) {
		return;
	}

	std::ifstream ifs(fnOld, std::ios::binary);
	if (!ifs) {
		CSTHROW(std::string("Failed to open \"") + fnOld + "\" for renaming (" + std::strerror(errno) + ")");
	}
	std::ofstream ofs(fnNew, std::ios::binary);
	if (!ofs) {
		CSTHROW(std::string("Failed to open \"") + fnNew + "\" as a rename target (" + std::strerror(errno) + ")");
	}

	ofs << ifs.rdbuf();
}

std::optional<uint64_t> fileSize(const char* path) {
	struct stat st;

	if (stat(path, &st) != 0) {
		return std::nullopt;
	}
	return st.st_size;
}

struct TmpFile::TmpFileImpl {
	std::fstream fs;
	std::string fname;
};

TmpFile::TmpFile(): impl(std::make_unique<TmpFileImpl>()) {
	char tmpName[] = "/tmp/tmp_XXXXXX";
	int fd;
	if ((fd = mkstemp(tmpName)) < 0) {
		throw std::runtime_error(std::string("Failed creating temporary file (") + std::strerror(errno) + ")");
	}
	this->impl->fname = std::string(tmpName);
	this->impl->fs.open(tmpName);
	close(fd);
	if (!this->impl->fs) {
		throw std::runtime_error(std::string("Failed opening temporary file (") + std::strerror(errno) + ")");
	}
}

TmpFile::~TmpFile() {
	this->impl->fs.close();
	std::remove(this->impl->fname.c_str());
}

std::fstream& TmpFile::fs() {
	return this->impl->fs;
}

const char* TmpFile::name() {
	return this->impl->fname.c_str();
}

void TmpFile::refresh() {
	this->impl->fs.close();
	this->impl->fs.open(this->impl->fname.c_str());
	if (!this->impl->fs) {
		throw std::runtime_error("Failed to refresh temporary file \"" + this->impl->fname + "\" (" + std::strerror(errno) + ")");
	}
}

}
