/** @file crypto/password.hpp
 * @brief Handles password key derivation and secure reading.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_CRYPTO_PASSWORD_HPP
#define __CS_CRYPTO_PASSWORD_HPP

#include "secbytes.hpp"
#include <memory>
#include <utility>

namespace CloudSync::Crypto {

enum KDFType {
	ARGON2 = 0,
	HKDF = 1,
	PBKDF2 = 2,
	SCRYPT = 3,
};

enum HashType {
	RIPEMD256 = 0,
	MD5 = 1,
	SHA1 = 2,
	SHA256 = 3,
	SHA3 = 4,
	SHA512 = 5,
};

/**
 * @brief Derives a key/iv pair from a password.
 *
 * @param password The password to derive from.
 * @param keyLen The length of the key that should be returned.
 * @param ivLen The length of the IV that should be returned.
 * @param kt The Key Derivation Function to use. By default this is HKDF.
 * @param ht The hash function to use while deriving. By default this is SHA256.
 *
 * @return A pair containing the Key (first) and IV (second).
 */
std::pair<SecBytes, SecBytes> DeriveKeypair(SecBytes password, size_t keyLen, size_t ivLen, KDFType kt = HKDF, HashType ht = SHA256);

/**
 * @brief Asks the user for a password and derives a key/iv pair from it.
 *
 * @param prompt The prompt to display to the user.
 * @param verify_prompt The prompt that verifies the password. This can be nullptr if verification is not necessary.
 * @param keyLen The length of the key that should be returned.
 * @param ivLen The length of the IV that should be returned.
 * @param kt The Key Derivation Function to use. By default this is HKDF.
 * @param ht The hash function to use while deriving. By default this is SHA256.
 *
 * @return A pair containing the Key (first) and IV (second).
 */
std::pair<SecBytes, SecBytes> StdinKeypair(const char* prompt, const char* verify_prompt, size_t keyLen, size_t ivLen, KDFType kt = HKDF, HashType ht = SHA256);

}

#endif
