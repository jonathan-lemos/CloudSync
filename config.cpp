/** @file config.cpp
 * @brief Config file creator/reader.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "config.hpp"
#include "filetools.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <list>
#include <memory>
#include <string>

namespace CloudSync{

/**
 * @brief All ConfigFile's begin with this magic constant.
 * This is how this class knows a ConfigFile is valid.
 *
 * A ConfigFile has the following format:
 * ```
 * CF
 * KEY\0<8-byte length><data>KEY2\0<8-byte length><data>...
 * ```
 * This way any kind of data can be stored, even those with newlines and null chars.
 * Furthermore, any kind of key can also be stored, as a null terminator is used as the delimiter.
 */
constexpr const char* CF_HEADER = "CF\n";

struct ConfigFile::ConfigFileImpl {
	/**
	 * @brief The path of the ConfigFile being edited.
	 */
	const char* path = nullptr;
	/**
	 * @brief True if there are pending changes, false if not.
	 */
	bool pending = false;

	/**
	 * @brief The entries in the ConfigFile.
	 * The first element of the pair is the key used to access it.
	 * The second element of the pair is the actual data as an array of bytes.
	 *
	 * This is a vector and not a linked list because binary search is used to access the entries.
	 */
	std::vector<std::pair<std::string, std::vector<unsigned char>>> entries = {};

	/**
	 * @brief Inserts an entry into the correct spot in the entry vector.
	 * The correct spot makes sure that this vector is sorted at all times.
	 * This function also removes an entry with a duplicate key.
	 *
	 * @param entry The entry to insert.
	 */
	void insertEntry(const std::pair<std::string, std::vector<unsigned char>>& entry) {
		// Iterate in reverse because more often than not this will insert at the back.
		auto pos = this->entries.end() - 1;
		for (; pos >= this->entries.begin(); --pos) {
			// If an entry already exists with this key.
			if (entry.first == pos->first) {
				// Remove it, then decrement the iterator
				this->entries.erase(pos--);
				break;
			}
			else if (entry.first < pos->first) {
				break;
			}
		}
		// Increment the iterator so we are at the correct spot to insert
		pos++;
		this->entries.insert(pos, entry);
		this->pending = true;
	}

	/**
	 * @brief Creates a file out of the current entry vector.
	 *
	 * @param path The path to create the file in.
	 * If a file already exists at this path, it will be overwritten on success.
	 * On failure this file is untouched.
	 *
	 * @exception std::runtime_error There was an I/O error writing to the file.
	 */
	void writeFile() {
		// If there are no pending entries, don't waste time.
		if (!this->pending) {
			return;
		}

		// Create a temp file so if there are errors, the original file is untouched.
		File::TmpFile tmpFileBuf;
		File::TmpFile tmpFileSave;
		std::fstream& fs = tmpFileBuf.fs();

		fs.write(CF_HEADER, strlen(CF_HEADER));
		std::for_each(this->entries.begin(), this->entries.end(), [&fs](const std::pair<std::string, std::vector<unsigned char>>& elem){
			const std::string& key = elem.first;
			const std::vector<unsigned char>& data = elem.second;
			uint64_t len = data.size();
			// Write the key including the terminating null.
			fs.write(key.c_str(), key.size() + 1);
			// Now write the length as a string of 8-bytes
			fs.write(reinterpret_cast<const char*>(&len), sizeof(len));
			// Finally, write the data to the file.
			fs.write(reinterpret_cast<const char*>(&(data[0])), len);
		});

		// If there were any I/O errors.
		fs.close();
		if (!fs.good()) {
			throw std::runtime_error(std::string("I/O error writing to file \"") + this->path + " (" + std::strerror(errno) + ")");
		}

		// Finally, replace the old file with the new one.
		tmpFileSave.fs().close();
		std::remove(tmpFileSave.name());
		if (std::rename(this->path, tmpFileSave.name()) != 0) {
			throw std::runtime_error(std::string("Failed to move old file to temporary directory (") + std::strerror(errno) + ")");
		}

		if (std::rename(tmpFileBuf.name(), this->path) != 0) {
			std::rename(tmpFileSave.name(), this->path);
			throw std::runtime_error(std::string("Failed to rename temporary file to final output (") + std::strerror(errno) + ")");
		}

		this->pending = false;
	}
};

ConfigFile::ConfigFile(const char* path): impl(std::make_unique<ConfigFileImpl>()) {
	std::ifstream ifs(path);
	char headerBuf[sizeof(CF_HEADER)];

	// If the file does not exist,
	if (!ifs.good()) {
		// Set entries to an empty vector.
		this->impl->entries = {};
		// Don't try to read the file.
		return;
	}

	// Read what should be the header into the header buffer.
	ifs.read(headerBuf, sizeof(headerBuf) - 1);
	// If the first n bytes of the file do not match the header.
	if (strcmp(headerBuf, CF_HEADER) != 0) {
		throw std::runtime_error(std::string("The file pointed to by \"") + path + "\" is not of the correct ConfigFile format");
	}

	// While there is data in the file to be read.
	while (ifs.peek() != EOF) {
		std::string key = "";
		char keyBuf[256];
		std::vector<unsigned char> data;
		uint64_t len;

		// Read until a '\0'
		// The loop makes sure we can read keys longer than 256 bytes.
		do {
			ifs.getline(keyBuf, sizeof(keyBuf), '\0');
			key += keyBuf;
		} while (strlen(keyBuf) == sizeof(keyBuf) - 1);

		if (ifs.eof()) {
			throw std::runtime_error(std::string("The file pointed to by \"") + path + "\" has a corrupted key.");
		}

		// Now read the 8-byte length into len.
		ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
		if (ifs.eof()) {
			throw std::runtime_error(std::string("The file pointed to by \"") + path + "\" has a corrupted length for key \"" + key + "\"");
		}

		// Now that we know the length, read the data into the vector.
		data.resize(len);
		ifs.read(reinterpret_cast<char*>(&(data[0])), len);
		if (!ifs.good()) {
			throw std::runtime_error(std::string("I/O error while reading file \"") + path + "\"");
		}

		// Finally, add this entry to our vector.
		this->impl->insertEntry(std::make_pair(key, data));
	}
}

ConfigFile::ConfigFile(ConfigFile&& other) {
	this->impl = std::move(other.impl);
}

ConfigFile& ConfigFile::operator=(ConfigFile&& other) {
	this->impl = std::move(other.impl);
	return *this;
}

ConfigFile& ConfigFile::writeEntry(const char* key, void* data, uint64_t data_len) {
	// Create a vector out of the data
	unsigned char* ptr = static_cast<unsigned char*>(data);
	std::vector<unsigned char> vec(ptr, ptr + data_len);
	// Insert it into the list
	this->impl->insertEntry(std::make_pair(key, vec));
	return *this;
}

std::optional<std::reference_wrapper<const std::vector<unsigned char>>> CS_PURE ConfigFile::readEntry(const char* key) const {
	// Since the list is sorted, we can use binary search here.
	size_t left = 0;
	size_t right = this->impl->entries.size() - 1;
	while (left <= right) {
		size_t mid = (left + right) / 2;
		int res = strcmp(this->impl->entries[mid].first.c_str(), key);
		if (res < 0) {
			left = right + 1;
		}
		else if (res > 0) {
			right = left - 1;
		}
		else {
			return this->impl->entries[mid].second;
		}
	}
	return std::nullopt;
}

void ConfigFile::flush() {
	this->impl->writeFile();
}

ConfigFile::~ConfigFile() {
	this->flush();
}

}
