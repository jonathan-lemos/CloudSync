/** @file file.cpp
 * @brief Various file functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "file.hpp"
#include "existsexception.hpp"
#include "ioexception.hpp"
#include "notfoundexception.hpp"
#include "../lnthrow.hpp"
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>

namespace CloudSync::fs {

Type getType(const char* path) {
	try {
		if (!exists(path)) {
			return Type::NotFound;
		}
		if (isDirectory(path)) {
			return Type::Directory;
		}
		if (isFile(path)) {
			return Type::File;
		}
		if (isSymlink(path)) {
			return Type::Symlink;
		}
		return Type::NotFound;
	}
	catch (IOException& e) {
		lnthrow(IOException, std::string("Failed to get the type of \"") + path + "\"", e);
	}
}

bool isDirectory(const char* path) {
	try {
		return std::filesystem::is_directory(path);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to determine if \"") + path + "\" is a directory", e);
	}
}

bool isFile(const char* path) {
	try {
		return std::filesystem::is_regular_file(path);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to determine if \"") + path + "\" is a file", e);
	}
}

bool isSymlink(const char* path) {
	try {
		return std::filesystem::is_symlink(path);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to determine if \"") + path + "\" is a symlink", e);
	}
}

bool exists(const char* path) {
	try {
		return std::filesystem::exists(path);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to determine if \"") + path + "\" exists.");
	}
}

uint64_t size(const char* path) {
	if (!exists(path)) {
		lnthrow(NotFoundException, std::string("\"") + path + "\" does not exist.");
	}
	if (!isFile(path)) {
		lnthrow(NotFoundException, std::string("\"") + path + "\" is not a file.");
	}

	try {
		return std::filesystem::file_size(path);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to determine the file size of \"") + path + "\"", e);
	}
}

void move(const char* src, const char* dst) {
	if (std::filesystem::equivalent(src, dst)) {
		return;
	}
	if (exists(dst)) {
		lnthrow(ExistsException, std::string("Move destination \"") + dst + "\" already exists");
	}

	try {
		std::filesystem::rename(src, dst);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to move \"") + src + "\" to \"" + dst + "\".", e);
	}
}

void copy(const char* src, const char* dst) {
	if (std::filesystem::equivalent(src, dst)) {
		return;
	}
	if (!exists(src)) {
		lnthrow(NotFoundException, std::string("Copy source \"") + src + "\" does not exist");
	}
	if (exists(dst)) {
		lnthrow(ExistsException, std::string("Copy destination \"") + dst + "\" already exists");
	}

	if (isDirectory(src)) {
		try {
			std::filesystem::copy(src, dst, std::filesystem::copy_options::copy_symlinks | std::filesystem::copy_options::recursive);
		}
		catch (std::filesystem::filesystem_error& e) {
			lnthrow(IOException, std::string("Failed to copy directory \"") + src + "\" to destination \"" + dst + "\"", e);
		}
	}
	if (isFile(src)) {
		try {
			std::filesystem::copy(src, dst, std::filesystem::copy_options::copy_symlinks);
		}
		catch (std::filesystem::filesystem_error& e) {
			lnthrow(IOException, std::string("Failed to copy file \"") + src + "\" to destination \"" + dst + "\"", e);
		}
	}
}

bool remove(const char* path) {
	if (!exists(path)) {
		return false;
	}
	try {
		std::filesystem::remove_all(path);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to remove path \"") + path + "\"");
	}
	return true;
}

void createSymlink(const char* path, const char* target) {
	if (std::filesystem::equivalent(path, target)) {
		lnthrow(ExistsException, std::string("Cannot symlink \"") + path + "\" to itself.");
	}
	if (exists(path)) {
		lnthrow(ExistsException, std::string("Symlink path \"") + path + "\" already exists");
	}

	try {
		std::filesystem::create_symlink(target, path);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to create symlink at \"") + path + "\" with target \"" + target + "\"", e);
	}
}

bool createDirectory(const char* path) {
	if (exists(path)) {
		if (isDirectory(path)) {
			return false;
		}
		else {
			lnthrow(ExistsException, std::string("A file/symlink already exists at the path \"") + path + "\"");
		}
	}

	try {
		std::filesystem::create_directories(path);
	}
	catch (std::filesystem::filesystem_error& e) {
		lnthrow(IOException, std::string("Failed to create directory \"") + path + "\"", e);
	}
	return true;
}

std::pair<std::string, std::ofstream> makeTemp(const char* baseDir) {
	const std::filesystem::path base = baseDir ? baseDir : std::filesystem::temp_directory_path();
	std::filesystem::path file;
	std::ofstream fs;

	do {
		const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
		char tmpname[11];

		for (char& c : tmpname) {
			c = alphabet[std::rand() % sizeof(alphabet) - 1];
		}
		tmpname[sizeof(tmpname) - 1] = '\0';

		file = base;
		file /= "tmp_";
		file += tmpname;
	} while (exists(file.c_str()));

	fs.open(file.c_str(), std::ios_base::out | std::ios_base::binary);
	if (!fs) {
		lnthrow(IOException, std::string("Failed to create temp file \"") + file.c_str() + "\" (" + std::strerror(errno) + ")");
	}
	return std::make_pair(std::string(file.c_str()), std::move(fs));
}

std::string parentDir(const char* dir) {
	return std::filesystem::path(dir).parent_path().generic_u8string();
}

}
