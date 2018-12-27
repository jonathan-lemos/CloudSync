/** @file crypto/symmetric.cpp
 * @brief Handles symmetric encryption.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "password.hpp"
#include "symmetric.hpp"
#include <cryptopp/aes.h>
#include <cryptopp/blowfish.h>
#include <cryptopp/camellia.h>
#include <cryptopp/cast.h>
#include <cryptopp/ccm.h>
#include <cryptopp/eax.h>
#include <cryptopp/gcm.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <memory>
#include <variant>

namespace CloudSync::Crypto {

const char* bcToString(BlockCipher bc) {
	switch (bc) {
	case AES:
		return "AES";
	case BLOWFISH:
		return "Blowfish";
	case CAMELLIA:
		return "Camellia";
	case CAST6:
		return "CAST6";
	}
}

const char* cmToString(CipherMode cm) {
	switch (cm) {
	case CCM:
		return "CCM";
	case CBC:
		return "CBC";
	case CFB:
		return "CFB";
	case CTR:
		return "CTR";
	case EAX:
		return "EAX";
	case GCM:
		return "GCM";
	}
}

std::variant<std::unique_ptr<CryptoPP::CipherModeBase>, std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>> getEncCipher(enum BlockCipher bc, enum CipherMode cm) {
	switch (bc) {
	case AES:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::CCM<CryptoPP::AES>::Encryption());
		case CBC:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption());
		case CFB:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption());
		case CTR:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption());
		case EAX:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::EAX<CryptoPP::AES>::Encryption());
		case GCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::GCM<CryptoPP::AES>::Encryption());
		}
	case BLOWFISH:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::CCM<CryptoPP::Blowfish>::Encryption());
		case CBC:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption());
		case CFB:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Encryption());
		case CTR:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Encryption());
		case EAX:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::EAX<CryptoPP::Blowfish>::Encryption());
		case GCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::GCM<CryptoPP::Blowfish>::Encryption());
		}
	case CAMELLIA:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::CCM<CryptoPP::Camellia>::Encryption());
		case CBC:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CBC_Mode<CryptoPP::Camellia>::Encryption());
		case CFB:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CFB_Mode<CryptoPP::Camellia>::Encryption());
		case CTR:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CTR_Mode<CryptoPP::Camellia>::Encryption());
		case EAX:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::EAX<CryptoPP::Camellia>::Encryption());
		case GCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::GCM<CryptoPP::Camellia>::Encryption());
		}
	case CAST6:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::CCM<CryptoPP::CAST256>::Encryption());
		case CBC:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CBC_Mode<CryptoPP::CAST256>::Encryption());
		case CFB:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CFB_Mode<CryptoPP::CAST256>::Encryption());
		case CTR:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CTR_Mode<CryptoPP::CAST256>::Encryption());
		case EAX:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::EAX<CryptoPP::CAST256>::Encryption());
		case GCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::GCM<CryptoPP::CAST256>::Encryption());
		}
	}
}

std::variant<std::unique_ptr<CryptoPP::CipherModeBase>, std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>> getDecCipher(enum BlockCipher bc, enum CipherMode cm) {
	switch (bc) {
	case AES:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::CCM<CryptoPP::AES>::Decryption());
		case CBC:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption());
		case CFB:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption());
		case CTR:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption());
		case EAX:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::EAX<CryptoPP::AES>::Decryption());
		case GCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::GCM<CryptoPP::AES>::Decryption());
		}
	case BLOWFISH:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::CCM<CryptoPP::Blowfish>::Decryption());
		case CBC:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Decryption());
		case CFB:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Decryption());
		case CTR:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Decryption());
		case EAX:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::EAX<CryptoPP::Blowfish>::Decryption());
		case GCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::GCM<CryptoPP::Blowfish>::Decryption());
		}
	case CAMELLIA:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::CCM<CryptoPP::Camellia>::Decryption());
		case CBC:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CBC_Mode<CryptoPP::Camellia>::Decryption());
		case CFB:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CFB_Mode<CryptoPP::Camellia>::Decryption());
		case CTR:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CTR_Mode<CryptoPP::Camellia>::Decryption());
		case EAX:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::EAX<CryptoPP::Camellia>::Decryption());
		case GCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::GCM<CryptoPP::Camellia>::Decryption());
		}
	case CAST5:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::CCM<CryptoPP::CAST256>::Decryption());
		case CBC:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CBC_Mode<CryptoPP::CAST256>::Decryption());
		case CFB:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CFB_Mode<CryptoPP::CAST256>::Decryption());
		case CTR:
			return std::make_unique<CryptoPP::CipherModeBase>(CryptoPP::CTR_Mode<CryptoPP::CAST256>::Decryption());
		case EAX:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::EAX<CryptoPP::CAST256>::Decryption());
		case GCM:
			return std::make_unique<CryptoPP::AuthenticatedSymmetricCipherBase>(CryptoPP::GCM<CryptoPP::CAST256>::Decryption());
		}
	}
}

int getBlockSize(BlockCipher bc) {
	switch (bc) {
	case AES:
		return CryptoPP::AES::BLOCKSIZE;
	case BLOWFISH:
		return CryptoPP::Blowfish::BLOCKSIZE;
	case CAMELLIA:
		return CryptoPP::Camellia::BLOCKSIZE;
	case CAST6:
		return CryptoPP::CAST256::BLOCKSIZE;
	}
}

struct Encryptor::EncryptorImpl {
	SecBytes key;
	SecBytes iv;
	int keyLen;
	std::variant<std::unique_ptr<CryptoPP::CipherModeBase>, std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherDocumentation>> mode;
};

bool validateKeyLen(int keyLen, BlockCipher bc) {
	(void)bc;
	return keyLen == 128 || keyLen == 192 || keyLen == 256;
}

Encryptor::Encryptor(SecBytes password, BlockCipher bc, int keyLen, CipherMode cb): impl(std::make_unique<EncryptorImpl>()) {
	std::pair<SecBytes, SecBytes> keyPair = DeriveKeypair(password, keyLen, getBlockSize(bc));
	if (!validateKeyLen(keyLen, bc)) {
		throw std::logic_error("Key length " + std::to_string(keyLen) + " cannot be used with block cipher " + bcToString(bc));
	}
}

}

