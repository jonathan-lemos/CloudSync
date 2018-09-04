/** @file baseclient.hpp
 * @brief Base client class that needs to be implemented.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_BASECLIENT_HPP
#define __CS_BASECLIENT_HPP

#include <vector>
#include <string>

namespace CloudSync{

class BaseClient{
	public:
		virtual bool login(const char* username, const char* password);
		virtual bool mkdir(const char* dir);
		virtual bool readdir(const char* dir, std::vector<std::string>& out);
		virtual bool stat(const char* path, struct stat* st);
		virtual bool rename(const char* old_path, const char* new_path);
		virtual bool download(const char* cloud_path, const char* disk_path);
		virtual bool upload(const char* disk_path, const char* cloud_path);
		virtual bool remove(const char* path);
		virtual bool logout();
};

}

#endif
