/** @file config.hpp
 * @brief Config file creator/reader.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <vector>
#include <string>
#include <optional>
#include <memory>

namespace CloudSync{

struct ConfigFileImpl;

class ConfigFile{
public:
	static ConfigFile openForReading(const char* filename);
	static ConfigFile openForWriting(const char* filename);
	~ConfigFile();

	ConfigFile& writeEntry(std::string key, void* data, size_t data_len);
	/* and you thought java was verbose */
	std::optional<std::reference_wrapper<const std::vector<unsigned char>>> readEntry(std::string key);
private:
	ConfigFile();
	std::unique_ptr<ConfigFileImpl> impl;
};

}
