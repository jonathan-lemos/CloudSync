/** @file crypto/password.cpp
 * @brief Handles password key derivation and secure reading.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "password.hpp"
#include "../terminal.hpp"
#include <cryptopp/hkdf.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/ripemd.h>
#include <cryptopp/scrypt.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>

namespace CloudSync::Crypto {

static std::unique_ptr<CryptoPP::KeyDerivationFunction> getKdf(KDFType kt, HashType ht) {
	switch (kt) {
	case HKDF:
		switch (ht) {
		case RIPEMD256:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::HKDF<CryptoPP::RIPEMD256>());
		case SHA1:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::HKDF<CryptoPP::SHA1>());
		case SHA256:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::HKDF<CryptoPP::SHA256>());
		case SHA3:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::HKDF<CryptoPP::SHA3>());
		case SHA512:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::HKDF<CryptoPP::SHA512>());
		}
	case PBKDF2:
		switch (ht) {
		case RIPEMD256:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::RIPEMD256>());
		case SHA1:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1>());
		case SHA256:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256>());
		case SHA3:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA3>());
		case SHA512:
			return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512>());
		}
	case SCRYPT:
		return std::make_unique<CryptoPP::KeyDerivationFunction>(CryptoPP::Scrypt());
	}
}

std::pair<SecBytes, SecBytes> DeriveKeypair(SecBytes password, size_t keyLen, size_t ivLen, KDFType kt, HashType ht) {
	std::unique_ptr<CryptoPP::KeyDerivationFunction> kdf = getKdf(kt, ht);
	SecBytes buf(keyLen + ivLen);
	SecBytes key;
	SecBytes iv;

	kdf->DeriveKey(buf.data(), keyLen + ivLen, password.data(), password.size());
	key = SecBytes(buf.data(), keyLen);
	iv = SecBytes(buf.data() + keyLen, ivLen);
	return std::make_pair(key, iv);
}

}
