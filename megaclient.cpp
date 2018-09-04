#include "megaclient.hpp"
#include "keys.h"
#include "logger.hpp"

namespace CloudSync{

static std::string string_parent_dir(const char* in){
	std::string ret = in;
	size_t index;
	index = ret.find_last_of('/');
	if (index == std::string::npos){
		return "";
	}
	ret.resize(index + 1);
	return ret;
}

static std::string string_filename(const char* in){
	std::string ret = in;
	size_t index;
	index = ret.find_last_of('/');
	if (index == std::string::npos){
		return "";
	}
	ret = in + index + 1;
	return ret;
}

bool MegaClient::login(const char* username, const char* password){
	mega::SynchronousRequestListener srl;

	mapi = std::make_unique<mega::MegaApi>(MEGA_API_KEY, (const char*)NULL, "cloudsync");

	mapi->login(username, password, &srl);

	if (srl.trywait(MEGA_WAIT_MS) != 0){
		LOG(LEVEL_WARNING) << "MEGA: Connection timed out";
		lastError = timedOutError;
		return false;
	}
	if (srl.getError()->getErrorCode() != mega::MegaError::API_OK){
		LOG(LEVEL_ERROR) << "MEGA: Failed to login (" << srl.getError()->toString() << ")";
		lastError = srl.getError()->toString();
		return false;
	}

	mapi->fetchNodes(&srl);
	if (srl.trywait(MEGA_WAIT_MS) != 0){
		LOG(LEVEL_WARNING) << "MEGA: Connection timed out";
		lastError = timedOutError;
		return false;
	}
	if (srl.getError()->getErrorCode() != mega::MegaError::API_OK){
		LOG(LEVEL_ERROR) << "MEGA: Failed to fetch nodes (" << srl.getError()->toString() << ")";
		lastError = srl.getError()->toString();
		return false;
	}

	return true;
}

const char* MegaClient::getLastError(){
	if (lastError){
		return (*lastError).c_str();
	}
	return NULL;
}

}
