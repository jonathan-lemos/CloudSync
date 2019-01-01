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
#include <optional>
#include <memory>

namespace CloudSync{

/**
 * @brief An error code from the MegaClient.
 * In most cases you should just use MegaClient::getLastError.
 */
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

class MegaClient final : public BaseClient{
public:
	MegaClient();
	~MegaClient();

	virtual bool login(const char* email, const char* password) override;
	virtual bool mkdir(const char* dir) override;
	virtual std::optional<std::vector<std::string>> readdir(const char* dir) override;
	virtual bool stat(const char* path, struct stat* st) override;
	virtual bool move(const char* oldPath, const char* newPath) override;
	virtual bool download(const char* cloudPath, const char* diskPath) override;
	virtual bool upload(const char* diskPath, const char* cloudPath) override;
	virtual bool remove(const char* path) override;
	virtual bool logout() override;

	const char* getLastError();
	MegaClientErrorCode getLastErrorCode();
	const char* getLastApiError();
	void setUploadMsg(const char* msg);
	void setDownloadMsg(const char* msg);
private:
	struct MegaClientImpl;
	std::unique_ptr<MegaClientImpl> impl;
};

}

#endif
