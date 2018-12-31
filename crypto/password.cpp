/** @file crypto/password.cpp
 * @brief Handles password key derivation and secure reading.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "password.hpp"
#include "../terminal.hpp"
// the following import does not work unless this one is present
#include <cryptopp/algparam.h>
#include <cryptopp/hkdf.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/ripemd.h>
#include <cryptopp/scrypt.h>
#include <cryptopp/sha.h>
#include <iostream>

namespace CloudSync::Crypto {

static std::unique_ptr<CryptoPP::KeyDerivationFunction> getKdf(KDFType kt, HashType ht) {
	switch (kt) {
	case HKDF:
		switch (ht) {
		case RIPEMD256:
			return std::make_unique<CryptoPP::HKDF<CryptoPP::RIPEMD256>>(CryptoPP::HKDF<CryptoPP::RIPEMD256>());
		case SHA1:
			return std::make_unique<CryptoPP::HKDF<CryptoPP::SHA1>>(CryptoPP::HKDF<CryptoPP::SHA1>());
		case SHA256:
			return std::make_unique<CryptoPP::HKDF<CryptoPP::SHA256>>(CryptoPP::HKDF<CryptoPP::SHA256>());
		case SHA512:
			return std::make_unique<CryptoPP::HKDF<CryptoPP::SHA512>>(CryptoPP::HKDF<CryptoPP::SHA512>());
		default:
			throw std::runtime_error("Switch statement fell through when all enum cases were covered.");
		}
	case PBKDF2:
		switch (ht) {
		case RIPEMD256:
			return std::make_unique<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::RIPEMD256>>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::RIPEMD256>());
		case SHA1:
			return std::make_unique<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1>>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1>());
		case SHA256:
			return std::make_unique<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256>>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256>());
		case SHA512:
			return std::make_unique<CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512>>(CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA512>());
		default:
			throw std::runtime_error("Switch statement fell through when all enum cases were covered.");
		}
	case SCRYPT:
		return std::make_unique<CryptoPP::Scrypt>(CryptoPP::Scrypt());
	}
	throw std::runtime_error("Switch statement fell through when all enum cases were covered.");
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

std::optional<std::pair<SecBytes, SecBytes>> StdinKeypair(const char* prompt, const char* verify_prompt, size_t keyLen, size_t ivLen, KDFType kt, HashType ht) {
	constexpr size_t bufLen = 256;
	SecBytes input;
	SecBytes buf;
	std::pair<SecBytes, SecBytes> ret;

	Terminal::echo(false);
	std::cout << prompt;
	do {
		buf.resize(bufLen);
		fgets(reinterpret_cast<char*>(buf.data()), bufLen, stdin);
		buf.resize(std::strlen(reinterpret_cast<char*>(buf.data())));
		input += buf;
	} while (buf.size() == bufLen - 1);
	Terminal::echo(true);

	ret = DeriveKeypair(input, keyLen, ivLen, kt, ht);
	if (!verify_prompt) {
		return ret;
	}
	input.resize(0);

	Terminal::echo(false);
	std::cout << prompt;
	do {
		buf.resize(bufLen);
		fgets(reinterpret_cast<char*>(buf.data()), bufLen, stdin);
		buf.resize(std::strlen(reinterpret_cast<char*>(buf.data())));
		input += buf;
	} while (buf.size() == bufLen - 1);
	Terminal::echo(true);

	std::pair<SecBytes, SecBytes> tmp = DeriveKeypair(input, keyLen, ivLen, kt, ht);
	if (ret.first != tmp.first || ret.second != tmp.second) {
		return std::nullopt;
	}
	return ret;
}

}
