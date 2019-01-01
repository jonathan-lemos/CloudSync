/** @file megaclient.cpp
 * @brief Mega.nz client class.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "attribute.hpp"
#include "megaclient.hpp"
#include "keys.hpp"
#include "logger.hpp"
#include "progressbar.hpp"
#include "sdk/mega_sdk/include/megaapi.h"

#include <sys/stat.h>
#include <cstring>
#include <unistd.h>
#include <mutex>
#include <condition_variable>

namespace CloudSync{

class MegaClientError{
public:
	MegaClientError(): mcec(NO_ERROR), apiError(nullptr) {}

	const char* toString(){
		switch (mcec){
		case NO_ERROR:
			return nullptr;
		case TIMED_OUT:
			return "The request timed out";
		case PATH_EXISTS:
			return "The path already exists";
		case INVALID_PATH:
			return "The path was invalid";
		case IS_FILE:
			return "The path specifies a file";
		case IS_DIRECTORY:
			return "The path specifies a directory";
		case PATH_NOT_FOUND:
			return "The path does not exist";
		case REQUEST_ERROR:
			if (!apiError){
				return "Unknown request error. This should never happen";
			}
			return std::string(std::string("Request error: ") + apiError).c_str();
		case TRANSFER_ERROR:
			if (!apiError){
				return "Unknown transfer error. This should never happen";
			}
			return std::string(std::string("Transfer error: ") + apiError).c_str();
		case SHOULDNEVERHAPPEN_ERROR:
			return "This should never happen.";
		}
		return nullptr;
	}

	MegaClientErrorCode getErrorCode(){
		return mcec;
	}

	const char* getApiError(){
		return apiError;
	}

	void setError(MegaClientErrorCode mcec, const char* apiError = nullptr){
		this->mcec = mcec;
		if ((mcec == REQUEST_ERROR || mcec == TRANSFER_ERROR) &&
				apiError == nullptr){
			throw std::invalid_argument("mcec was REQUEST_ERROR or TRANSFER_ERROR, but apiError was not specified");
		}
		this->apiError = apiError;
	}

private:
	enum MegaClientErrorCode mcec;
	const char* apiError;
};

struct MegaClient::MegaClientImpl{
	const char* uploadMsg = nullptr;
	const char* downloadMsg = nullptr;
	std::unique_ptr<mega::MegaApi> mapi = nullptr;
	MegaClientError lastError;
};

class ProgressBarTransferListener : public mega::MegaTransferListener{
public:
	ProgressBarTransferListener(): notified(false), error(nullptr), transfer(nullptr), p(){}

	void setMsg(const char* msg){
		p.setMsg(msg);
	}

	void onTransferStart(mega::MegaApi* mega_api, mega::MegaTransfer* transfer){
		struct stat st;

		(void)mega_api;

		switch(transfer->getType()){
		case mega::MegaTransfer::TYPE_UPLOAD:
			if (stat(transfer->getFileName(), &st) != 0){
				LOG(LEVEL_DEBUG) << "MEGA: Could not deduce file size for " << transfer->getFileName();
				break;
			}
			p.setMax(st.st_size);
			p.display();
			break;

		case mega::MegaTransfer::TYPE_DOWNLOAD:
			p.setMax(transfer->getTotalBytes());
			p.display();
			break;

		default:
			LOG(LEVEL_DEBUG) << "MEGA: Could not start progress bar due to unknown transfer type.";
		}
	}

	void onTransferUpdate(mega::MegaApi* mega_api, mega::MegaTransfer* transfer){
		(void)mega_api;
		p.setProgress(transfer->getTransferredBytes());
	}

	void onTransferTemporaryError(mega::MegaApi* mega_api, mega::MegaTransfer* transfer, mega::MegaError* error){
		(void)mega_api;
		(void)transfer;
		LOG(LEVEL_DEBUG) << "MEGA: Transfer Temporary Error: " << error->toString();
	}

	void onTransferFinish(mega::MegaApi* mega_api, mega::MegaTransfer* transfer, mega::MegaError* error){
		(void)mega_api;

		this->error = std::unique_ptr<mega::MegaError>(error->copy());
		this->transfer = std::unique_ptr<mega::MegaTransfer>(transfer->copy());

		if (error->getErrorCode() == mega::MegaError::API_OK){
			p.finish();
		}
		else{
			p.fail();
		}

		{
			std::unique_lock<std::mutex> lock(m);
			notified = true;
		}
		cv.notify_all();
	}

	bool trywait(int millis){
		std::unique_lock<std::mutex> lock(m);
		cv.wait_for(lock, std::chrono::milliseconds(millis), [this]{return notified;});
		return notified;
	}

	void wait(){
		std::unique_lock<std::mutex> lock(m);
		cv.wait(lock, [this]{return notified;});
	}

	void reset(){
		p.reset();

		transfer = nullptr;
		error = nullptr;
		notified = false;
	}

	mega::MegaTransfer* getTransfer(){
		return transfer.get();
	}

	mega::MegaError* getError(){
		return error.get();
	}

private:
	bool notified = false;
	std::unique_ptr<mega::MegaError> error = nullptr;
	std::unique_ptr<mega::MegaTransfer> transfer = nullptr;
	std::condition_variable cv;
	std::mutex m;
	ProgressBar p;
	MegaClientError err;
};

static std::optional<std::string> CS_PURE string_parent_dir(const char* in){
	std::string ret = in;
	size_t index;
	index = ret.find_last_of('/');
	if (index == std::string::npos){
		return {};
	}
	ret.resize(index + 1);
	return ret;
}

static std::optional<std::string> CS_PURE string_filename(const char* in){
	std::string ret = in;
	size_t index;
	index = ret.find_last_of('/');
	if (index == std::string::npos){
		return {};
	}
	ret = in + index + 1;
	return ret;
}

MegaClient::~MegaClient(){
	if (impl->mapi){
		logout();
	}
}

bool MegaClient::login(const char* username, const char* password){
	mega::SynchronousRequestListener srl;

	if (impl->mapi != nullptr){
		return false;
	}

	impl->mapi = std::make_unique<mega::MegaApi>(MEGA_API_KEY, (const char*)NULL, "cloudsync");

	impl->mapi->login(username, password, &srl);
	if (srl.trywait(MEGA_WAIT_MS) != 0){
		impl->lastError.setError(TIMED_OUT);
		return false;
	}
	if (srl.getError()->getErrorCode() != mega::MegaError::API_OK){
		impl->lastError.setError(REQUEST_ERROR, srl.getError()->toString());
		return false;
	}

	impl->mapi->fetchNodes(&srl);
	if (srl.trywait(MEGA_WAIT_MS) != 0){
		impl->lastError.setError(TIMED_OUT);
		return false;
	}
	if (srl.getError()->getErrorCode() != mega::MegaError::API_OK){
		LOG(LEVEL_ERROR) << "MEGA: Failed to fetch nodes (" << srl.getError()->toString() << ")";
		impl->lastError.setError(REQUEST_ERROR, srl.getError()->toString());
		return false;
	}

	return true;
}

bool MegaClient::mkdir(const char* dir){
	std::optional<std::string> parent_path;
	std::optional<std::string> filename;
	std::unique_ptr<mega::MegaNode> node;
	mega::SynchronousRequestListener srl;

	node = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(dir));
	if (node){
		impl->lastError.setError(PATH_EXISTS);
		return false;
	}

	parent_path = string_parent_dir(dir);
	if (!parent_path){
		impl->lastError.setError(INVALID_PATH);
		return false;
	}

	filename = string_filename(dir);
	if (!filename){
		impl->lastError.setError(INVALID_PATH);
		return false;
	}

	node = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(parent_path.value().c_str()));
	if (!node){
		impl->lastError.setError(PATH_NOT_FOUND);
		return false;
	}

	if (node->isFile()){
		impl->lastError.setError(IS_FILE);
		return false;
	}

	impl->mapi->createFolder(filename.value().c_str(), node.get(), &srl);
	if (srl.trywait(MEGA_WAIT_MS) != 0){
		impl->lastError.setError(TIMED_OUT);
		return false;
	}

	if (srl.getError()->getErrorCode() != mega::MegaError::API_OK){
		impl->lastError.setError(REQUEST_ERROR, srl.getError()->toString());
		return false;
	}
	return true;
}

std::optional<std::vector<std::string>> MegaClient::readdir(const char* dir){
	std::vector<std::string> ret;
	std::unique_ptr<mega::MegaNode> node;
	std::unique_ptr<mega::MegaNodeList> children;

	node = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(dir));
	if (!node){
		impl->lastError.setError(PATH_NOT_FOUND);
		return std::nullopt;
	}
	if (node->isFile()){
		impl->lastError.setError(IS_FILE);
		return std::nullopt;
	}

	children = std::unique_ptr<mega::MegaNodeList>(impl->mapi->getChildren(node.get()));
	for (int i = 0; i < children->size(); ++i){
		ret.push_back(children->get(i)->getName());
	}
	return ret;
}

bool MegaClient::stat(const char* path, struct stat* st){
	std::unique_ptr<mega::MegaNode> node(impl->mapi->getNodeByPath(path));

	if (!node){
		impl->lastError.setError(PATH_NOT_FOUND);
		return false;
	}
	if (!st){
		return true;
	}

	st->st_uid = getuid();
	st->st_gid = getgid();
	st->st_mode = node->isFile() ? S_IFREG | 0444 : S_IFDIR | 0755;
	st->st_nlink = 1;
	st->st_mtime = node->isFile() ? node->getModificationTime() : node->getCreationTime();
	st->st_ctime = node->getCreationTime();

	return true;
}

bool MegaClient::move(const char* old_path, const char* new_path){
	std::unique_ptr<mega::MegaNode> nSrc;
	std::unique_ptr<mega::MegaNode> nDst;
	std::unique_ptr<mega::MegaNode> nTmp;
	std::optional<std::string> parent_path;
	std::optional<std::string> filename;
	mega::SynchronousRequestListener srl;

	if (strcmp(old_path, new_path) == 0){
		return true;
	}

	nSrc = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(old_path));
	if (!nSrc){
		impl->lastError.setError(PATH_NOT_FOUND);
		return false;
	}

	nDst = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(new_path));
	if (!nDst){
		parent_path = string_parent_dir(new_path);
		filename = string_filename(new_path);
		if (!parent_path || !filename){
			impl->lastError.setError(INVALID_PATH);
			return false;
		}

		nDst = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(parent_path.value().c_str()));
		if (!nDst){
			impl->lastError.setError(PATH_NOT_FOUND);
			return false;
		}
		if (nDst->isFile()){
			impl->lastError.setError(IS_FILE);
			return false;
		}

		nTmp = std::unique_ptr<mega::MegaNode>(impl->mapi->getChildNode(nDst.get(), filename.value().c_str()));
		if (nTmp){
			impl->lastError.setError(PATH_EXISTS);
			return false;
		}
	}
	if (nDst->isFile()){
		impl->lastError.setError(PATH_EXISTS);
		return false;
	}

	impl->mapi->moveNode(nSrc.get(), nDst.get(), &srl);
	if (srl.trywait(MEGA_WAIT_MS) != 0){
		impl->lastError.setError(TIMED_OUT);
		return false;
	}
	if (srl.getError()->getErrorCode() != mega::MegaError::API_OK){
		impl->lastError.setError(REQUEST_ERROR, srl.getError()->toString());
		return false;
	}

	return true;
}

bool MegaClient::download(const char* cloud_path, const char* disk_path){
	std::unique_ptr<mega::MegaNode> node;
	ProgressBarTransferListener pbtl;

	node = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(cloud_path));
	if (!node){
		impl->lastError.setError(PATH_NOT_FOUND);
		return false;
	}
	if (!node->isFile()){
		impl->lastError.setError(IS_DIRECTORY);
		return false;
	}

	pbtl.setMsg(impl->uploadMsg);
	impl->mapi->startDownload(node.get(), disk_path, &pbtl);
	pbtl.wait();
	if (pbtl.getError()->getErrorCode() != mega::MegaError::API_OK){
		impl->lastError.setError(TRANSFER_ERROR, pbtl.getError()->toString());
		return false;
	}
	return true;
}

bool MegaClient::upload(const char* disk_path, const char* cloud_path){
	std::unique_ptr<mega::MegaNode> node;
	ProgressBarTransferListener pbtl;
	std::optional<std::string> parent_dir;

	node = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(cloud_path));
	if (node && node->isFile()){
		impl->lastError.setError(PATH_EXISTS);
		return false;
	}

	if (!node){
		parent_dir = string_parent_dir(cloud_path);
		if (!parent_dir ||
				!(node = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(parent_dir.value().c_str())))){
			impl->lastError.setError(PATH_NOT_FOUND);
			return false;
		}
	}

	pbtl.setMsg(impl->downloadMsg);
	impl->mapi->startUpload(disk_path, node.get(), &pbtl);
	pbtl.wait();
	if (pbtl.getError()->getErrorCode() != mega::MegaError::API_OK){
		impl->lastError.setError(TRANSFER_ERROR, pbtl.getError()->toString());
		return false;
	}

	if (parent_dir){
		std::unique_ptr<mega::MegaNode> nUploaded;
		mega::SynchronousRequestListener srl;

		std::string f_old = parent_dir.value();
		std::string f_new;
		if (f_old[f_old.length() - 1] != '/'){
			f_old += '/';
		}
		f_new = f_old;
		f_old += string_filename(disk_path).value();
		f_new += string_filename(cloud_path).value();

		nUploaded = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(f_old.c_str()));
		if (!nUploaded){
			impl->lastError.setError(SHOULDNEVERHAPPEN_ERROR);
			return false;
		}

		impl->mapi->renameNode(nUploaded.get(), f_new.c_str(), &srl);
		if (srl.trywait(MEGA_WAIT_MS) != 0){
			impl->lastError.setError(TIMED_OUT);
		}
		if (srl.getError()->getErrorCode() != mega::MegaError::API_OK){
			impl->lastError.setError(REQUEST_ERROR, srl.getError()->toString());
		}
	}
	return true;
}

bool MegaClient::remove(const char* path){
	std::unique_ptr<mega::MegaNode> node;
	mega::SynchronousRequestListener srl;

	node = std::unique_ptr<mega::MegaNode>(impl->mapi->getNodeByPath(path));
	if (!node){
		impl->lastError.setError(PATH_NOT_FOUND);
	}

	impl->mapi->remove(node.get(), &srl);
	if (srl.trywait(MEGA_WAIT_MS) != 0){
		impl->lastError.setError(TIMED_OUT);
		return false;
	}
	if (srl.getError()->getErrorCode() != mega::MegaError::API_OK){
		impl->lastError.setError(REQUEST_ERROR, srl.getError()->toString());
		return false;
	}

	return true;
}

bool MegaClient::logout(){
	mega::SynchronousRequestListener srl;
	int res;

	if (impl->mapi == nullptr){
		return true;
	}

	impl->mapi->logout(&srl);
	res = srl.trywait(MEGA_WAIT_MS);
	impl->mapi = nullptr;

	if (res != 0){
		impl->lastError.setError(TIMED_OUT);
		impl->mapi = nullptr;
		return false;
	}
	return true;
}

const char* MegaClient::getLastError(){
	return impl->lastError.toString();
}

MegaClientErrorCode MegaClient::getLastErrorCode(){
	return impl->lastError.getErrorCode();
}

const char* MegaClient::getLastApiError(){
	return impl->lastError.getApiError();
}

void MegaClient::setUploadMsg(const char* msg){
	this->impl->uploadMsg = msg;
}

void MegaClient::setDownloadMsg(const char* msg){
	this->impl->downloadMsg = msg;
}

}
