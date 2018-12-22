/** @file os/linux/filetools.cpp
 * @brief Functions for dealing with files.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../../filetools.hpp"
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

std::optional<uint64_t> fileSize(const char* path) {
	struct stat st;

	if (stat(path, &st) != 0) {
		return std::nullopt;
	}
	return st.st_size;
}

std::optional<std::pair<std::string, std::fstream>> openTmp() {
	char tmpName[] = "/tmp/tmp_XXXXXX";
	int fd;
	if ((fd = mkstemp(tmpName)) < 0) {
		return std::nullopt;
	}

	std::fstream f;
	f.open(tmpName, std::ios::in | std::ios::out | std::ios::binary);
	close(fd);
	if (!f.good()) {
		return std::nullopt;
	}

	return std::make_pair(std::string(tmpName), f);
}

}
