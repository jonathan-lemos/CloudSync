/** @file config.hpp
 * @brief Config file creator/reader.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CONFIG_HPP
#define __CONFIG_HPP

#include "attribute.hpp"
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>


namespace CloudSync{

/**
 * @brief Writes/reads to/from a config file.
 */
class ConfigFile {
public:
	/**
	 * @brief Creates a new ConfigFile at the given path.
	 * If a file does not exist at this path, it will be created.
	 *
	 * @param filename The path of the config file to open.
	 *
	 * @exception std::runtime_error The given file already exists and is not of the correct format.
	 */
	ConfigFile(const char* path);

	/**
	 * @brief Move constructor for a ConfigFile.
	 */
	ConfigFile(ConfigFile&& other);

	/**
	 * @brief Deleted copy constructor for a ConfigFile.
	 * ConfigFiles are not meant to be copied.
	 */
	ConfigFile(const ConfigFile& other) = delete;

	/**
	 * @brief Move assignment operator for a ConfigFile.
	 */
	ConfigFile& operator=(ConfigFile&& other);

	/**
	 * @brief Deleted copy assignment operator for a ConfigFile.
	 * ConfigFiles are not meant to be copied.
	 */
	ConfigFile& operator=(const ConfigFile& other) = delete;

	/**
	 * @brief Closes the internal fstream and flushes any pending changes.
	 */
	~ConfigFile();

	/**
	 * @brief Writes an entry to the file.
	 * These can be retrieved with ConfigFile::readEntry() later.
	 * @see CloudSync::ConfigFile::readEntry()
	 *
	 * @param key The key that will be used to refer to the data.
	 * If a key with this entry already exists, it will be overwritten.
	 * At the moment, the key can contain any character.
	 * For future compatibillity, restrict the key to [A-Za-z0-9].
	 *
	 * @param data The data to write.
	 * @param data_len The length of the data to write.
	 *
	 * @return this
	 *
	 * @exception std::logic_error The ConfigFile is opened for reading instead of writing.
	 * @exception std::runtime_error I/O error when writing to the file.
	 */
	ConfigFile& writeEntry(const char* key, void* data, uint64_t data_len);

	/**
	 * @brief Retrieves the data corresponding to the given key from the file.
	 * To create these entries, use ConfigFile::writeEntry().
	 *
	 * @param key The key to retrieve.
	 *
	 * @return A reference to a byte vector, or std::nullopt if the key could not be found.
	 */
	std::optional<std::reference_wrapper<const std::vector<unsigned char>>> readEntry(const char* key) const;

	/**
	 * @brief Removes a key from the file.
	 *
	 * @param key The key to remove.
	 *
	 * @return True if the key was removed, false if the key did not exist in the file.
	 */
	bool removeEntry(const char* key);

	/**
	 * @brief Gets a vector containing all the entries in the file.
	 *
	 * @return A reference to a vector containing all the entries in the file.
	 * The first member of the pair is the key.
	 * The second member of the pair is the data.
	 */
	const std::vector<std::pair<std::string, std::vector<unsigned char>>>& getAllEntries() const;

	/**
	 * @brief Flushes the current unwritten changes to the buffer.
	 *
	 * @exception std::runtime_error There was an I/O error writing to the file.
	 */
	void flush();

private:
	struct ConfigFileImpl;
	/**
	 * @brief A pointer to the private variables and inner workings of the ConfigFile class.
	 */
	std::unique_ptr<ConfigFileImpl> impl;
};

}

#endif
