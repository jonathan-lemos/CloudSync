/** @file baseclient.hpp
 * @brief Base client class that needs to be implemented.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_BASECLIENT_HPP
#define __CS_BASECLIENT_HPP

#include <sys/stat.h>
#include <vector>
#include <string>

namespace CloudSync{

class BaseClient{
	public:
		virtual ~BaseClient();
		virtual bool login(const char* username, const char* password) = 0;
		virtual bool mkdir(const char* dir) = 0;
		virtual bool readdir(const char* dir, std::vector<std::string>& out) = 0;
		virtual bool stat(const char* path, struct stat* st) = 0;
		virtual bool rename(const char* old_path, const char* new_path) = 0;
		virtual bool download(const char* cloud_path, const char* disk_path) = 0;
		virtual bool upload(const char* disk_path, const char* cloud_path) = 0;
		virtual bool remove(const char* path) = 0;
		virtual bool logout() = 0;
};

}

#endif
