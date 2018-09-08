/** @file config.cpp
 * @brief Config file creator/reader.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "config.hpp"
#include <stdexcept>
#include <tuple>
#include <algorithm>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <memory>

namespace CloudSync{

class ReadingConfigFile{
public:
	ReadingConfigFile(const char* filename){
		std::ifstream ifs;
		ifs.open(filename);
		if (!ifs){
			throw std::runtime_error("Failed to open config file " + std::string(filename) + " for reading (" + std::strerror(errno) + ")");
		}

		while (ifs){
			std::string key = "";
			size_t len;
			std::vector<unsigned char> vec;
			char buf[256];

			do{
				ifs.getline(buf, sizeof(buf), '\0');
				key += buf;
			}while (std::strlen(buf) == sizeof(buf) - 1);

			ifs.read((char*)&len, sizeof(len));
			if (ifs.eof()){
				throw std::runtime_error(std::string(filename) + " is not of the correct config file format.");
			}
			vec.resize(len);
			ifs.read((char*)&(vec[0]), len);
			readVec.push_back(std::make_pair(key, vec));
		}

		ifs.close();
		if (ifs.fail()){
			throw std::runtime_error("Read error");
		}
	}

	std::optional<std::reference_wrapper<const std::vector<unsigned char>>> readEntry(std::string key){
		size_t left = 0;
		size_t right = readVec.size();
		while (left <= right){
			size_t mid = (left + right) / 2;
			std::string& val = readVec[mid].first;
			int cmp_val = key.compare(val);

			if (cmp_val < 0){
				right = mid - 1;
			}
			else if (cmp_val > 0){
				left = mid + 1;
			}
			else{
				return std::make_optional(readVec[mid].second);
			}
		}
		return std::nullopt;
	}

private:
	std::vector<std::pair<std::string, std::vector<unsigned char>>> readVec;
};

class WritingConfigFile{
public:
	WritingConfigFile(const char* filename){
		ofs.open(filename);
		if (!ofs){
			throw std::runtime_error("Failed to open config file " + std::string(filename) + " for writing (" + std::strerror(errno) + ")");
		}
	}

	void writeEntry(std::string key, void* data, size_t len){
		ofs.write(key.c_str(), key.size());
		ofs.put('\0');
		ofs.write((char*)&(len), sizeof(len));
		ofs.write((char*)data, len);
		if (!ofs){
			throw std::runtime_error("Failed to write to config file.");
		}
	}

	~WritingConfigFile(){
		ofs.close();
	}

private:
	std::ofstream ofs;
	std::vector<std::tuple<std::string, void*, size_t>> writeVec;
};

struct ConfigFileImpl{
	bool writing;
	union{
		ReadingConfigFile rcf;
		WritingConfigFile wcf;
	};
};

ConfigFile::ConfigFile(): impl(std::make_unique<ConfigFileImpl>()){}

ConfigFile& ConfigFile::writeEntry(std::string key, void* data, size_t data_len){
	if (!impl->writing){
		throw std::logic_error("ConfigFile is opened in reading mode, but writeEntry() was called.");
	}
	impl->wcf.writeEntry(key, data, data_len);
	return *this;
}

std::optional<std::reference_wrapper<const std::vector<unsigned char>>> ConfigFile::readEntry(std::string key){
	if (impl->writing){
		throw std::logic_error("ConfigFile is opened in writing mode, but readEntry() was called.");
	}
	return impl->rcf.readEntry(key);
}

ConfigFile::~ConfigFile(){
	if (impl->writing){
		impl->wcf.~WritingConfigFile();
	}
	else{
		impl->rcf.~ReadingConfigFile();
	}
}

}
