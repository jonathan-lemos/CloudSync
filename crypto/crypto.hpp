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
	CAST5 = 3,
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

class Encryptor {
	Encryptor(SecBytes password, BlockCipher bc = AES, int keySize = 256, CipherMode cb = GCM);
	Encryptor(SecBytes key, SecBytes iv, BlockCipher bc = AES, int keySize = 256, CipherMode cb = GCM);
	~Encryptor();

private:
	struct EncryptorImpl;
	std::unique_ptr<EncryptorImpl> impl;
};

}

#endif
