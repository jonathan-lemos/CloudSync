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

enum class BlockCipher {
	AES = 0,
	BLOWFISH = 1,
	CAMELLIA = 2,
	CAST6 = 3,
	RIJNDAEL = 0,
};

enum class CipherMode {
	CCM = 0,
	CBC = 1,
	CFB = 2,
	CTR = 3,
	EAX = 4,
	GCM = 5,
};

class Symmetric {
public:
	Symmetric(const char* password, BlockCipher bc = BlockCipher::AES, int keySize = 256, CipherMode cb = CipherMode::GCM);
	Symmetric(const char* key, const SecBytes& iv, BlockCipher bc = BlockCipher::AES, int keySize = 256, CipherMode cb = CipherMode::GCM);
	void encryptData(const unsigned char* in, size_t inLen, unsigned char* out, size_t outLen) const;
	void encryptFile(const char* filenameIn, const char* filenameOut) const;
	void encryptFile(const char* filenameInOut) const;
	~Symmetric() noexcept;

private:
	struct SymmetricImpl;
	std::unique_ptr<SymmetricImpl> impl;
};

}

#endif
