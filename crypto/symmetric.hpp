/** @file crypto/symmetric.hpp
 * @brief Handles symmetric encryption.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_CRYPTO_SYMMETRIC_HPP
#define __CS_CRYPTO_SYMMETRIC_HPP

#include "secbytes.hpp"
#include <memory>

namespace CloudSync::Crypto {

enum BlockCipher {
	AES = 0,
	BLOWFISH = 1,
	CAMELLIA = 2,
	CAST6 = 3,
	RIJNDAEL = 0,
};

enum CipherMode {
	CCM = 0,
	CBC = 1,
	CFB = 2,
	CTR = 3,
	EAX = 4,
	GCM = 5,
};

class Symmetric {
	Symmetric(const char* password, BlockCipher bc = AES, int keySize = 256, CipherMode cb = GCM);
	Symmetric(const char* key, const SecBytes& iv, BlockCipher bc = AES, int keySize = 256, CipherMode cb = GCM);
	void encryptData(const unsigned char* in, size_t inLen, unsigned char* out, size_t outLen);
	void encryptFile(const char* filenameIn, const char* filenameOut);
	void encryptFile(const char* filenameInOut);
	~Symmetric();

private:
	struct SymmetricImpl;
	std::unique_ptr<SymmetricImpl> impl;
};

}

#endif
