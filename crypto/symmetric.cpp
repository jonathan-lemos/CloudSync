/** @file crypto/symmetric.cpp
 * @brief Handles symmetric encryption.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "symmetric.hpp"
#include "../fs/file.hpp"
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
#include <fstream>
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
	default:
		throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
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
	default:
		throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
	}
}

std::variant<std::unique_ptr<CryptoPP::CipherModeBase>, std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>> getEncCipher(enum BlockCipher bc, enum CipherMode cm) {
	switch (bc) {
	case AES:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::AES>::Encryption>(CryptoPP::CCM<CryptoPP::AES>::Encryption());
		case CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption>(CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption());
		case CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption>(CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption());
		case CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption>(CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption());
		case EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::AES>::Encryption>(CryptoPP::EAX<CryptoPP::AES>::Encryption());
		case GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::AES>::Encryption>(CryptoPP::GCM<CryptoPP::AES>::Encryption());
		default:
			throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
		}
	case BLOWFISH:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::Blowfish>::Encryption>(CryptoPP::CCM<CryptoPP::Blowfish>::Encryption());
		case CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption>(CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption());
		case CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Encryption>(CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Encryption());
		case CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Encryption>(CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Encryption());
		case EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::Blowfish>::Encryption>(CryptoPP::EAX<CryptoPP::Blowfish>::Encryption());
		case GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::Blowfish>::Encryption>(CryptoPP::GCM<CryptoPP::Blowfish>::Encryption());
		default:
			throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
		}
	case CAMELLIA:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::Camellia>::Encryption>(CryptoPP::CCM<CryptoPP::Camellia>::Encryption());
		case CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::Camellia>::Encryption>(CryptoPP::CBC_Mode<CryptoPP::Camellia>::Encryption());
		case CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::Camellia>::Encryption>(CryptoPP::CFB_Mode<CryptoPP::Camellia>::Encryption());
		case CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::Camellia>::Encryption>(CryptoPP::CTR_Mode<CryptoPP::Camellia>::Encryption());
		case EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::Camellia>::Encryption>(CryptoPP::EAX<CryptoPP::Camellia>::Encryption());
		case GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::Camellia>::Encryption>(CryptoPP::GCM<CryptoPP::Camellia>::Encryption());
		default:
			throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
		}
	case CAST6:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::CAST256>::Encryption>(CryptoPP::CCM<CryptoPP::CAST256>::Encryption());
		case CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::CAST256>::Encryption>(CryptoPP::CBC_Mode<CryptoPP::CAST256>::Encryption());
		case CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::CAST256>::Encryption>(CryptoPP::CFB_Mode<CryptoPP::CAST256>::Encryption());
		case CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::CAST256>::Encryption>(CryptoPP::CTR_Mode<CryptoPP::CAST256>::Encryption());
		case EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::CAST256>::Encryption>(CryptoPP::EAX<CryptoPP::CAST256>::Encryption());
		case GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::CAST256>::Encryption>(CryptoPP::GCM<CryptoPP::CAST256>::Encryption());
		default:
			throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
		}
	}
	throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
}

std::variant<std::unique_ptr<CryptoPP::CipherModeBase>, std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>> getDecCipher(enum BlockCipher bc, enum CipherMode cm) {
	switch (bc) {
	case AES:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::AES>::Decryption>(CryptoPP::CCM<CryptoPP::AES>::Decryption());
		case CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption>(CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption());
		case CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption>(CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption());
		case CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption>(CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption());
		case EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::AES>::Decryption>(CryptoPP::EAX<CryptoPP::AES>::Decryption());
		case GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::AES>::Decryption>(CryptoPP::GCM<CryptoPP::AES>::Decryption());
		default:
			throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
		}
	case BLOWFISH:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::Blowfish>::Decryption>(CryptoPP::CCM<CryptoPP::Blowfish>::Decryption());
		case CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Decryption>(CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Decryption());
		case CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Decryption>(CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Decryption());
		case CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Decryption>(CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Decryption());
		case EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::Blowfish>::Decryption>(CryptoPP::EAX<CryptoPP::Blowfish>::Decryption());
		case GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::Blowfish>::Decryption>(CryptoPP::GCM<CryptoPP::Blowfish>::Decryption());
		default:
			throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
		}
	case CAMELLIA:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::Camellia>::Decryption>(CryptoPP::CCM<CryptoPP::Camellia>::Decryption());
		case CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::Camellia>::Decryption>(CryptoPP::CBC_Mode<CryptoPP::Camellia>::Decryption());
		case CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::Camellia>::Decryption>(CryptoPP::CFB_Mode<CryptoPP::Camellia>::Decryption());
		case CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::Camellia>::Decryption>(CryptoPP::CTR_Mode<CryptoPP::Camellia>::Decryption());
		case EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::Camellia>::Decryption>(CryptoPP::EAX<CryptoPP::Camellia>::Decryption());
		case GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::Camellia>::Decryption>(CryptoPP::GCM<CryptoPP::Camellia>::Decryption());
		default:
			throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
		}
	case CAST6:
		switch (cm) {
		case CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::CAST256>::Decryption>(CryptoPP::CCM<CryptoPP::CAST256>::Decryption());
		case CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::CAST256>::Decryption>(CryptoPP::CBC_Mode<CryptoPP::CAST256>::Decryption());
		case CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::CAST256>::Decryption>(CryptoPP::CFB_Mode<CryptoPP::CAST256>::Decryption());
		case CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::CAST256>::Decryption>(CryptoPP::CTR_Mode<CryptoPP::CAST256>::Decryption());
		case EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::CAST256>::Decryption>(CryptoPP::EAX<CryptoPP::CAST256>::Decryption());
		case GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::CAST256>::Decryption>(CryptoPP::GCM<CryptoPP::CAST256>::Decryption());
		default:
			throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
		}
	}
	throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
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
	default:
		throw std::runtime_error("Switch case fell through when all enums have been exhausted.");
	}
}

struct Symmetric::SymmetricImpl {
	SecBytes key;
	SecBytes iv;
	std::variant<std::unique_ptr<CryptoPP::CipherModeBase>, std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>> mode;
};

bool validateKeyLen(int keyLen, BlockCipher bc) {
	(void)bc;
	return keyLen == 128 || keyLen == 192 || keyLen == 256;
}

Symmetric::Symmetric(const char* password, BlockCipher bc, int keyLen, CipherMode cb): impl(std::make_unique<SymmetricImpl>()) {
	std::pair<SecBytes, SecBytes> keyPair = DeriveKeypair(password, keyLen, getBlockSize(bc));
	if (!validateKeyLen(keyLen, bc)) {
		throw std::logic_error("Key length " + std::to_string(keyLen) + " cannot be used with block cipher " + bcToString(bc));
	}
	this->impl->key = keyPair.first;
	this->impl->iv = keyPair.second;
	this->impl->mode = getEncCipher(bc, cb);
}

void Symmetric::encryptData(const unsigned char* in, size_t inLen, unsigned char* out, size_t outLen) {
	const auto processCipherModeBase = [this](const unsigned char* in, size_t inLen, unsigned char* out) {
		std::get<std::unique_ptr<CryptoPP::CipherModeBase>>(this->impl->mode)->ProcessData(out, in, inLen);
	};

	const auto processAuthenticatedSymmetricCipherBase = [this](const unsigned char* in, size_t inLen, unsigned char* out) {
		std::get<std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>>(this->impl->mode)->ProcessData(out, in, inLen);
	};

	if (inLen != outLen) {
		throw new std::runtime_error("inLen (" + std::to_string(inLen) + ") does not equal outLen (" + std::to_string(outLen) + ")");
	}

	if (std::holds_alternative<std::unique_ptr<CryptoPP::CipherModeBase>>(this->impl->mode)) {
		processCipherModeBase(in, inLen, out);
	}
	else {
		processAuthenticatedSymmetricCipherBase(in, inLen, out);
	}
}

void Symmetric::encryptFile(const char* filenameIn, const char* filenameOut) {
	std::ifstream ifs;
	std::ofstream ofs;
	size_t len;

	ifs.open(filenameIn);
	if (!ifs) {
		throw std::runtime_error(std::string("Failed to open input file \"") + filenameIn + "\" (" + std::strerror(errno) + ")");
	}
	ofs.open(filenameOut);
	if (!ofs) {
		throw std::runtime_error(std::string("Failed to open output file \"") + filenameOut + "\" (" + std::strerror(errno) + ")");
	}

	do {
		unsigned char buf[65536];
		len = ifs.readsome(reinterpret_cast<char*>(buf), sizeof(buf));
		this->encryptData(buf, len, buf, len);
		ofs.write(reinterpret_cast<char*>(buf), len);
	} while (len > 0);

	if (!ifs) {
		throw std::runtime_error(std::string("Input file I/O error: ") + std::strerror(errno));
	}
	if (!ofs) {
		throw std::runtime_error(std::string("Output file I/O error: ") + std::strerror(errno));
	}
}

void Symmetric::encryptFile(const char* filenameInOut) {
	std::ifstream ifs;
	std::pair<std::string, std::fstream> tmpFile = fs::makeTemp();
	File::TmpFile tmpFileBuf;
	File::TmpFile tmpFileSave;

	tmpFileBuf.fs().close();
	std::remove(tmpFileBuf.name());
	this->encryptFile(filenameInOut, tmpFileBuf.name());

	if (std::rename(filenameInOut, tmpFileSave.name()) != 0) {
		throw std::runtime_error(std::string("Failed to move old file to temporary directory (") + std::strerror(errno) + ")");
	}

	if (std::rename(tmpFileBuf.name(), filenameInOut) != 0) {
		std::rename(tmpFileSave.name(), filenameInOut);
		throw std::runtime_error(std::string("Failed to rename temporary file to final output (") + std::strerror(errno) + ")");
	}
}

}

