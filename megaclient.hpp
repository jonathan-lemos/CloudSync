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

enum MegaClientErrorCode{
	NO_ERROR,
	TIMED_OUT,
	PATH_EXISTS,
	INVALID_PATH,
	IS_FILE,
	IS_DIRECTORY,
	PATH_NOT_FOUND,
	REQUEST_ERROR,
	TRANSFER_ERROR,
	SHOULDNEVERHAPPEN_ERROR
};

class MegaClientError{
public:
	MegaClientError();
	const char* toString();
	MegaClientErrorCode getErrorCode();
	const char* getApiError();
	void setError(MegaClientErrorCode mcec, const char* apiError = nullptr);


private:
	enum MegaClientErrorCode mcec;
	const char* apiError;
};


class MegaClient : public BaseClient{
public:
	~MegaClient();
	bool login(const char* email, const char* password);
	bool mkdir(const char* dir);
	bool readdir(const char* dir, std::vector<std::string>& out);
	bool stat(const char* path, struct stat* st);
	bool rename(const char* oldPath, const char* newPath);
	bool download(const char* cloudPath, const char* diskPath);
	bool upload(const char* diskPath, const char* cloudPath);
	bool remove(const char* path);
	bool logout();

	const char* getLastError();
	MegaClientErrorCode getLastErrorCode();
	const char* getLastApiError();
	void setUploadMsg(const char* msg);
	void setDownloadMsg(const char* msg);
private:
	const char* uploadMsg = nullptr;
	const char* downloadMsg = nullptr;
	std::unique_ptr<mega::MegaApi> mapi = nullptr;
	MegaClientError lastError;
};

}

#endif
