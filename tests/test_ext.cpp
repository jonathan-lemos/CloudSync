/** @file tests/test_ext.cpp
 * @brief Common functions for testing.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "test_ext.hpp"
#include "../fs/ioexception.hpp"
#include "../lnthrow.hpp"
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <cstdlib>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <vector>
#include <unistd.h>

#define BUFFER_LEN (65536)

namespace fs = std::filesystem;
namespace ex = CloudSync::fs;

namespace TestExt {

/**
 * @brief Gets the size in bytes of a file.
 *
 * @param filename The filename to get the size of.
 *
 * @return The size of the file at filename.
 * Note that this function does not detect folders, and will return 4096 bytes for those.
 *
 * @exception IOException Failed to stat the file.
 */
static long fileSize(const char* filename) {
	try {
		return fs::file_size(filename);
	}
	catch (fs::filesystem_error& e) {
		lnthrow(ex::IOException, std::string("Error statting \"") + filename + "\"", e);
	}
}

void createFile(const char* filename, const void* data, size_t dataLen) {
	std::ofstream ofs(filename);
	if (!ofs) {
		lnthrow(ex::IOException, std::string("Failed to open \"") + filename + "\" (" + std::strerror(errno) + ")");
	}
	ofs.write(reinterpret_cast<const char*>(data), dataLen);
	if (!ofs) {
		lnthrow(ex::IOException, std::string("I/O error writing to \"") + filename + "\"");
	}
}

int compare(const char* filename, const void* data, long dataLen) {
	long size = fileSize(filename);
	char buf[BUFFER_LEN];
	std::ifstream ifs;
	size_t len;
	size_t ptr = 0;

	if (size != dataLen) {
		return size - dataLen;
	}

	ifs.open(filename);
	if (!ifs) {
		lnthrow(ex::IOException, std::string("Failed to open \"") + filename + "\" (" + std::strerror(errno));
	}

	do {
		int res;
		len = ifs.readsome(buf, sizeof(buf));
		res = std::memcmp(buf, reinterpret_cast<const char*>(data) + ptr, len);
		if (res != 0) {
			return res;
		}
		ptr += len;
	} while (len > 0);

	return 0;
}

int compare(const char* filename, const std::vector<unsigned char>& vec) {
	return compare(filename, vec.data(), vec.size());
}

int compare(const char* filename, const char* otherFilename) {
	long size1 = fileSize(filename);
	long size2 = fileSize(otherFilename);
	char buf1[BUFFER_LEN];
	char buf2[BUFFER_LEN];
	std::ifstream ifs1;
	std::ifstream ifs2;
	size_t len1;
	size_t len2;

	if (size1 != size2) {
		return size1 - size2;
	}

	ifs1.open(filename);
	if (!ifs1) {
		lnthrow(ex::IOException, std::string("Failed to open \"") + filename + "\" (" + std::strerror(errno) + ")");
	}
	ifs2.open(otherFilename);
	if (!ifs2) {
		lnthrow(ex::IOException, std::string("Failed to open \"") + otherFilename + "\" (" + std::strerror(errno) + ")");
	}

	do {
		int res;
		len1 = ifs1.readsome(buf1, sizeof(buf1));
		len2 = ifs2.readsome(buf2, sizeof(buf2));
		if (len1 != len2) {
			return (int)len1 - (int)len2;
		}
		res = memcmp(buf1, buf2, len1);
		if (res != 0) {
			return res;
		}
	} while (len1 > 0);

	return 0;
}

bool fileExists(const char* filename) {
	struct stat st;
	if (stat(filename, &st) != 0) {
		return false;
	}
	return S_ISREG(st.st_mode);
}

bool dirExists(const char* dirname) {
	struct stat st;
	if (stat(dirname, &st) != 0) {
		return false;
	}
	return S_ISDIR(st.st_mode);
}

void fillData(void* data, size_t len) {
	unsigned char* ptr = reinterpret_cast<unsigned char*>(data);
	for (size_t i = 0; i < len; ++i) {
		ptr[i] = (i % 10) + '0';
	}
}

/**
* @brief Creates a path out of components:
* For example:
* ```
* makePath({"/boot", "syslinux", "syslinux.cfg"}) -> "/boot/syslinux/syslinux.cfg"
* ```
* @return The completed path as a string.
* A leading/ending '/' is not appended, but all intermediate '/''s are.
*/
static std::string makePath(std::vector<const char*> vec) {
	std::string s = "";
	std::for_each(vec.begin(), vec.end(), [&s](const auto& elem) {
		s += elem;
		if (s[s.length() - 1] == '/') {
			s.resize(s.length() - 1);
		}
		s += "/";
	});
	s.resize(s.length() - 1);
	return s;
}

struct TestEnvironment::TestEnvironmentImpl {
	/**
	 * @brief The internal file hashset.
	 */
	std::unordered_set<std::string> files;

	/**
	 * @brief The internal directory hashset.
	 */
	std::unordered_set<std::string> dirs;

	/**
	 * @brief The base directory of this TestEnvironment.
	 */
	std::string basePath;

	/**
	 * @brief Creates a directory and adds it to the internal dir/file hashsets.
	 * This directory is filled with files according to the parameters.
	 *
	 * @param path The path to create the directory in.
	 * This function will fail if the path already exists.
	 * @param prefix The prefix given to the files.
	 * @param suffix The suffix given to the files.
	 * The files together have the format: [prefix][number][suffix.
	 * @param nFiles The number of files to include.
	 * @param maxFileLen The maximum length of the files to make.
	 * The files will be filled with consistent data.
	 * @param seed The random seed used to determine the file sizes.
	 */
	void makeDirectory(const char* path, int nFiles = 20, const char* prefix = "test", const char* suffix = ".txt", int maxFileLen = 4096, unsigned seed = 0) {
		if (mkdir(path, 0755) != 0) {
			lnthrow(ex::IOException, std::string("Failed to create directory \"") + path + "\" (" + std::strerror(errno) + ")");
		}
		this->dirs.insert(path);

		srand(seed);
		for (int i = 0; i < nFiles; ++i) {
			std::string fname = makePath({path, std::string(prefix + std::to_string(i) + suffix).c_str()});
			size_t len = rand() % (maxFileLen + 1);
			std::vector<unsigned char> data;
			data.resize(len);
			fillData(&data[0], len);
			createFile(fname.c_str(), &data[0], len);
			this->files.insert(fname);
		}
	}
};

TestEnvironment::TestEnvironment(): impl(std::make_unique<TestEnvironmentImpl>()) {}

TestEnvironment TestEnvironment::Basic(const char* basePath, int nFiles, int maxFileLen) {
	TestEnvironment te;
	te.impl->basePath = basePath;
	te.impl->makeDirectory(basePath, nFiles, "test", ".txt", maxFileLen);
	return te;
}

TestEnvironment TestEnvironment::Full(const char* basePath, int nFilesPerDir, int maxFileLen) {
	constexpr const char* noaccDir = "noaccess";
	constexpr const char* noaccFile = "noaccess.txt";
	constexpr const char* noaccContents = "abcdefg_noaccess";
	std::string noaccDirPath = makePath({basePath, noaccDir});
	std::string noaccFilePath = makePath({basePath, noaccDir, noaccFile});
	TestEnvironment te;
	te.impl->basePath = basePath;
	te.impl->makeDirectory(basePath, nFilesPerDir, "test", ".txt", maxFileLen);
	te.impl->makeDirectory(makePath({basePath, "dir1"}).c_str(), nFilesPerDir, "d1_", ".txt", maxFileLen);
	te.impl->makeDirectory(makePath({basePath, "dir2"}).c_str(), nFilesPerDir, "d2_", ".txt", maxFileLen);
	te.impl->makeDirectory(makePath({basePath, "excl"}).c_str(), nFilesPerDir, "ex_", ".txt", maxFileLen);
	if (mkdir(noaccDirPath.c_str(), 0755) != 0) {
		lnthrow(ex::IOException, std::string("Failed to create directory \"") + noaccDirPath + "\" (" + std::strerror(errno) + ")");
	}
	te.impl->dirs.insert(noaccDirPath);
	createFile(noaccFilePath.c_str(), noaccContents, std::strlen(noaccContents));
	if (chmod(noaccFilePath.c_str(), 0000) != 0) {
		lnthrow(ex::IOException, std::string("Failed to chmod \"") + noaccFilePath.c_str() + "\" (" + std::string(std::strerror(errno)) + ")");
	}
	if (chmod(noaccDirPath.c_str(), 0000) != 0) {
		lnthrow(ex::IOException, std::string("Failed to chmod dir \"") + noaccDirPath.c_str() + "\" (" + std::string(std::strerror(errno)) + ")");
	}

	return te;
}

const std::unordered_set<std::string>& TestEnvironment::getFiles() {
	return this->impl->files;
}

const std::unordered_set<std::string>& TestEnvironment::getDirs() {
	return this->impl->dirs;
}

void rmRf(const char* basePath) {
	DIR* dp = opendir(basePath);
	struct dirent* dnt;

	while ((dnt = readdir(dp)) != nullptr) {
		std::string path = makePath({basePath, dnt->d_name});
		struct stat st;

		if (!strcmp(dnt->d_name, "..") || !strcmp(dnt->d_name, ".")) {
			continue;
		}

		stat(path.c_str(), &st);

		if (S_ISDIR(st.st_mode)) {
			chmod(path.c_str(), 0755);
			rmRf(path.c_str());
		}
		else {
			chmod(path.c_str(), 0644);
			std::remove(path.c_str());
		}
	}

	closedir(dp);
	rmdir(basePath);
}

TestEnvironment::~TestEnvironment() {
	rmRf(this->impl->basePath.c_str());
}

}
