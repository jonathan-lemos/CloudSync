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
	 * @brief Opens a ConfigFile at the given path.
	 * If a file does not exist at this path, it will be created.
	 *
	 * @param filename The path of the config file to open.
	 *
	 * @exception ExistsException The given file already exists and is not of the correct format.
	 */
	ConfigFile(const char* path);

	/**
	 * @brief Move constructor for a ConfigFile.
	 */
	ConfigFile(ConfigFile&& other) noexcept;

	/**
	 * @brief Deleted copy constructor for a ConfigFile.
	 * ConfigFiles are not meant to be copied.
	 */
	ConfigFile(const ConfigFile& other) = delete;

	/**
	 * @brief Move assignment operator for a ConfigFile.
	 */
	ConfigFile& operator=(ConfigFile&& other) noexcept;

	/**
	 * @brief Deleted copy assignment operator for a ConfigFile.
	 * ConfigFiles are not meant to be copied.
	 */
	ConfigFile& operator=(const ConfigFile& other) = delete;

	/**
	 * @brief Closes the internal fstream and flushes any pending changes.
	 */
	~ConfigFile() noexcept;

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
	 * @exception IOException I/O error when writing to the file.
	 */
	ConfigFile& writeEntry(const char* key, const void* data, uint64_t data_len) noexcept;

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
	 *
	 * @return this
	 *
	 * @exception IOException I/O error when writing to the file.
	 */
	ConfigFile& writeEntry(const char* key, const std::vector<unsigned char>& data) noexcept;

	/**
	 * @brief Retrieves the data corresponding to the given key from the file.
	 * To create these entries, use ConfigFile::writeEntry().
	 *
	 * @param key The key to retrieve.
	 *
	 * @return A reference to a byte vector, or std::nullopt if the key could not be found.
	 */
	std::optional<std::reference_wrapper<const std::vector<unsigned char>>> readEntry(const char* key) const noexcept;

	/**
	 * @brief Removes a key from the file.
	 *
	 * @param key The key to remove.
	 *
	 * @return True if the key was removed, false if the key did not exist in the file.
	 */
	bool removeEntry(const char* key) noexcept;

	/**
	 * @brief Gets a vector containing all the keys in the file.
	 *
	 * @return A vector containing all the keys in the file as strings.
	 */
	std::vector<std::string> getKeys() const noexcept;

	/**
	 * @brief Flushes the current unwritten changes to the buffer.
	 *
	 * @exception IOException There was an I/O error writing to the file.
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
