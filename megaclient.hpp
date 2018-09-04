/** @file megaclient.hpp
 * @brief Mega.nz client class.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_MEGACLIENT_HPP
#define __CS_MEGACLIENT_HPP

#ifndef MEGA_WAIT_MS
#define MEGA_WAIT_MS (10000)
#endif

#include "baseclient.hpp"
#include "mega_sdk/include/megaapi.h"
#include <optional>
#include <memory>

namespace CloudSync{

class MegaClient : BaseClient{
public:
	bool login(const char* email, const char* password);
	bool mkdir(const char* dir);
	bool readdir(const char* dir, std::vector<std::string>& out);
	bool stat(const char* path, struct stat* st);
	bool rename(const char* old_path, const char* new_path);
	bool upload(const char* disk_path, const char* cloud_path);
	bool remove(const char* path);
	bool logout();

	const char* getLastError();

private:
	/* and you thought java was verbose */
	static constexpr const char* const timedOutError = "Connection timed out";
	std::unique_ptr<mega::MegaApi> mapi;
	std::optional<std::string> lastError;
};

}

#endif
