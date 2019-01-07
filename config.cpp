/** @file config.cpp
 * @brief Config file creator/reader.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "config.hpp"
#include "fs/file.hpp"
#include "fs/ioexception.hpp"
#include "fs/existsexception.hpp"
#include "lnthrow.hpp"
#include "logger.hpp"
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
	std::string path;
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
	 * @brief Finds an entry within the entry vector.
	 *
	 * @param key The key to search for.
	 *
	 * @return An iterator to the entry, or entries.end() if the key does not exist within the entries vector.
	 */
	auto findEntry(const char* key) {
		std::string s(key);
		size_t left = 0;
		size_t right = entries.size() - 1;
		while (left <= right) {
			size_t mid = (left + right) / 2;
			auto it = entries.begin() + mid;

			if (it->first < s) {
				left = right + 1;
			}
			else if (it->first > s) {
				right = left - 1;
			}
			else {
				return it;
			}
		}
		return entries.end();
	}

	/**
	 * @brief Inserts an entry into the correct spot in the entry vector.
	 * The correct spot makes sure that this vector is sorted at all times.
	 * This function also removes an entry with a duplicate key.
	 *
	 * @param entry The entry to insert.
	 */
	void insertEntry(const std::pair<std::string, std::vector<unsigned char>>& entry) noexcept {
		// Iterate in reverse because more often than not this will insert at the back.
		auto pos = this->entries.rbegin();
		for (; pos != this->entries.rend(); ++pos) {
			// If an entry already exists with this key.
			if (entry.first == pos->first) {
				// Remove it, then decrement the iterator
				this->entries.erase((++pos).base());
				break;
			}
			else if (entry.first < pos->first) {
				break;
			}
		}
		this->entries.insert(pos.base(), entry);
		this->pending = true;
	}

	/**
	 * @brief Creates a file out of the current entry vector.
	 *
	 * @param path The path to create the file in.
	 * If a file already exists at this path, it will be overwritten on success.
	 * On failure this file is untouched.
	 *
	 * @exception CsError There was an I/O error writing to the file.
	 */
	void writeFile() {
		// If there are no pending entries, don't waste time.
		if (!this->pending) {
			return;
		}

		// Create a temp file so if there are errors, the original file is untouched.
		std::pair<std::string, std::fstream> tmpFileBuf = fs::makeTemp();
		std::pair<std::string, std::fstream> tmpFileSave = fs::makeTemp();
		std::fstream& fs = tmpFileBuf.second;

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
			lnthrow(fs::IOException, std::string("I/O error writing to file \"") + this->path + " (" + std::strerror(errno) + ")");
		}

		// Finally, replace the old file with the new one.
		try {
			fs::move(this->path.c_str(), tmpFileSave.first.c_str());

		}
		catch (fs::IOException& e) {
			lnthrow(fs::IOException, "I/O error replacing file \"" + this->path + "\" with temp file\"" + tmpFileBuf.first + "\"", e);
		}

		this->pending = false;
	}
};

ConfigFile::ConfigFile(const char* path): impl(std::make_unique<ConfigFileImpl>()) {
	std::ifstream ifs(path);
	char headerBuf[sizeof(CF_HEADER)];

	this->impl->path = path;

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
		lnthrow(fs::ExistsException, std::string("The file pointed to by \"") + path + "\" is not of the correct ConfigFile format");
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
			lnthrow(fs::ExistsException, std::string("The file pointed to by \"") + path + "\" has a corrupted key.");
		}

		// Now read the 8-byte length into len.
		ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
		if (ifs.eof()) {
			lnthrow(fs::ExistsException, std::string("The file pointed to by \"") + path + "\" has a corrupted length for key \"" + key + "\"");
		}

		// Now that we know the length, read the data into the vector.
		data.resize(len);
		ifs.read(reinterpret_cast<char*>(&(data[0])), len);
		if (!ifs.good()) {
			lnthrow(fs::IOException, std::string("I/O error while reading file \"") + path + "\"");
		}

		// Finally, add this entry to our vector.
		this->impl->insertEntry(std::make_pair(key, data));
	}
}

ConfigFile::ConfigFile(ConfigFile&& other) noexcept {
	this->impl = std::move(other.impl);
}

ConfigFile& ConfigFile::operator=(ConfigFile&& other) noexcept {
	this->impl = std::move(other.impl);
	return *this;
}

ConfigFile& ConfigFile::writeEntry(const char* key, const void* data, uint64_t data_len) noexcept {
	// Create a vector out of the data
	const unsigned char* ptr = static_cast<const unsigned char*>(data);
	std::vector<unsigned char> vec(ptr, ptr + data_len);
	// Insert it into the list
	this->impl->insertEntry(std::make_pair(key, vec));
	return *this;
}

ConfigFile& ConfigFile::writeEntry(const char* key, const std::vector<unsigned char>& data) noexcept {
	return this->writeEntry(key, data.data(), data.size());
}

std::optional<std::reference_wrapper<const std::vector<unsigned char>>> CS_PURE ConfigFile::readEntry(const char* key) const noexcept {
	auto it = this->impl->findEntry(key);
	if (it != this->impl->entries.end()) {
		return it->second;
	}
	return std::nullopt;
}

bool ConfigFile::removeEntry(const char* key) noexcept {
	auto it = this->impl->findEntry(key);
	if (it == this->impl->entries.end()) {
		return false;
	}
	this->impl->entries.erase(it);
	return true;
}

std::vector<std::string> ConfigFile::getKeys() const noexcept {
	std::vector<std::string> ret;
	std::transform(this->impl->entries.begin(), this->impl->entries.end(), std::back_inserter(ret), [](const auto& elem) {
		return elem.first;
	});
	return ret;
}

void ConfigFile::flush() {
	this->impl->writeFile();
}

ConfigFile::~ConfigFile() noexcept {
	try {
		this->flush();
	}
	catch (std::exception& e) {
		LOG(LEVEL_WARNING) << e.what();
	}
}

}
