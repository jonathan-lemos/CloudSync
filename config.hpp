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
#include <vector>
#include <string>
#include <optional>
#include <memory>


namespace CloudSync{

/**
 * @brief Writes/reads to/from a config file.
 */
class ConfigFile{
public:
	/**
	 * @brief Opens a config file for reading and reads all of its entries.
	 *
	 * @param filename The filename to read.
	 *
	 * @return A ConfigFile object corresponding to the specified file.
	 *
	 * @exception std::runtime_error Failed to open/read from the specified file.
	 * @exception std::invalid_argument The file was not of the correct format.
	 */
	static ConfigFile openForReading(const char* filename);

	/**
	 * @brief Opens a config file for writing.
	 *
	 * @param filename The filename to write to.
	 * If a file already exists at this location, it will be overwritten.
	 *
	 * @return A ConfigFile object corresponding to the specified file.
	 *
	 * @exception std::runtime_error Failed to open the specified file.
	 */
	void openForWriting(const char* filename);

	/**
	 * @brief Closes the internal fstream.
	 */
	~ConfigFile();

	/**
	 * @brief Writes an entry to the file.
	 * These can be retrieved with ConfigFile::readEntry() later.
	 * @see CloudSync::ConfigFile::readEntry()
	 *
	 * @param key The key that will be used to refer to the data.
	 * @param data The data to write.
	 * @param data_len The length of the data to write.
	 *
	 * @return this
	 *
	 * @exception std::logic_error The ConfigFile is opened for reading instead of writing.
	 * @exception std::runtime_error I/O error when writing to the file.
	 */
	ConfigFile& writeEntry(std::string key, void* data, size_t data_len);

	/**
	 * @brief Retrieves the data corresponding to the given key from the file.
	 * To create these entries, use ConfigFile::writeEntry().
	 *
	 * @param key The key to retrieve.
	 *
	 * @return A reference to the corresponding data as a vector, or std::nullopt if the key could not be found within the file.
	 *
	 * @exception std::logic_error The ConfigFile is opened for writing instead of reading.
	 */
	std::optional<std::reference_wrapper<const std::vector<unsigned char>>> CS_PURE readEntry(std::string key);

	friend void swap(ConfigFile& first, ConfigFile& second);

private:
	/**
	 * @brief Do not instantiate ConfigFile classes directly.
	 * Instead, use ConfigFile::openForReading() or ConfigFile::openForWriting() to retrieve an instance of this class.
	 */
	ConfigFile();

	struct ConfigFileImpl;
	/**
	 * @brief A pointer to the private variables and inner workings of the ConfigFile class.
	 */
	std::unique_ptr<ConfigFileImpl> impl;
};

}

#endif
