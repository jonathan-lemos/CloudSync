/** @file crypto/symmetric.cpp
 * @brief Handles symmetric encryption.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "crypto.hpp"
#include "password.hpp"
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

std::variant<CryptoPP::CipherModeDocumentation, CryptoPP::AuthenticatedSymmetricCipherDocumentation> getCipher(enum BlockCipher bc, enum CipherMode cm) {
	switch (bc) {
	case AES:
		switch (cm) {
			case CCM:
				return CryptoPP::CCM<CryptoPP::AES>();
			case CBC:
				return CryptoPP::CBC_Mode<CryptoPP::AES>();
			case CFB:
				return CryptoPP::CFB_Mode<CryptoPP::AES>();
			case CTR:
				return CryptoPP::CTR_Mode<CryptoPP::AES>();
			case EAX:
				return CryptoPP::EAX<CryptoPP::AES>();
			case GCM:
				return CryptoPP::GCM<CryptoPP::AES>();
		}
	case BLOWFISH:
		switch (cm) {
			case CCM:
				return CryptoPP::CCM<CryptoPP::Blowfish>();
			case CBC:
				return CryptoPP::CBC_Mode<CryptoPP::Blowfish>();
			case CFB:
				return CryptoPP::CFB_Mode<CryptoPP::Blowfish>();
			case CTR:
				return CryptoPP::CTR_Mode<CryptoPP::Blowfish>();
			case EAX:
				return CryptoPP::EAX<CryptoPP::Blowfish>();
			case GCM:
				return CryptoPP::GCM<CryptoPP::Blowfish>();
		}
	case CAMELLIA:
		switch (cm) {
			case CCM:
				return CryptoPP::CCM<CryptoPP::Camellia>();
			case CBC:
				return CryptoPP::CBC_Mode<CryptoPP::Camellia>();
			case CFB:
				return CryptoPP::CFB_Mode<CryptoPP::Camellia>();
			case CTR:
				return CryptoPP::CTR_Mode<CryptoPP::Camellia>();
			case EAX:
				return CryptoPP::EAX<CryptoPP::Camellia>();
			case GCM:
				return CryptoPP::GCM<CryptoPP::Camellia>();
		}
	case CAST5:
		switch (cm) {
			case CCM:
				return CryptoPP::CCM<CryptoPP::CAST128>();
			case CBC:
				return CryptoPP::CBC_Mode<CryptoPP::CAST128>();
			case CFB:
				return CryptoPP::CFB_Mode<CryptoPP::CAST128>();
			case CTR:
				return CryptoPP::CTR_Mode<CryptoPP::CAST128>();
			case EAX:
				return CryptoPP::EAX<CryptoPP::CAST128>();
			case GCM:
				return CryptoPP::GCM<CryptoPP::CAST128>();
		}
	}
}

struct Encryptor::EncryptorImpl {
	SecBytes key;
	SecBytes iv;
	CryptoPP::BlockCipherDocumentation cipher;
	std::variant<CryptoPP::CipherModeDocumentation, CryptoPP::AuthenticatedSymmetricCipherDocumentation> mode;
};

Encryptor::Encryptor(SecBytes password, BlockCipher bc, int keySize, CipherMode cb): impl(std::make_unique<EncryptorImpl>()) {
	this->cipher = getCipher(bc, cb);
	std::pair<SecBytes, SecBytes> sp = DeriveKeypair(password);
}

}

