/** @file crypto/symmetric.cpp
 * @brief Handles symmetric encryption.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "symmetric.hpp"
#include "../fs/file.hpp"
#include "../fs/ioexception.hpp"
#include "../lnthrow.hpp"
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
	case BlockCipher::AES:
		return "AES";
	case BlockCipher::BLOWFISH:
		return "Blowfish";
	case BlockCipher::CAMELLIA:
		return "Camellia";
	case BlockCipher::CAST6:
		return "CAST6";
	default:
		lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug."); // shut up gcc
	}
}

const char* cmToString(CipherMode cm) {
	switch (cm) {
	case CipherMode::CCM:
		return "CCM";
	case CipherMode::CBC:
		return "CBC";
	case CipherMode::CFB:
		return "CFB";
	case CipherMode::CTR:
		return "CTR";
	case CipherMode::EAX:
		return "EAX";
	case CipherMode::GCM:
		return "GCM";
	default:
		lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
	}
}

std::variant<std::unique_ptr<CryptoPP::CipherModeBase>, std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>> getEncCipher(enum BlockCipher bc, enum CipherMode cm) {
	switch (bc) {
	case BlockCipher::AES:
		switch (cm) {
		case CipherMode::CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::AES>::Encryption>(CryptoPP::CCM<CryptoPP::AES>::Encryption());
		case CipherMode::CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption>(CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption());
		case CipherMode::CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption>(CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption());
		case CipherMode::CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption>(CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption());
		case CipherMode::EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::AES>::Encryption>(CryptoPP::EAX<CryptoPP::AES>::Encryption());
		case CipherMode::GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::AES>::Encryption>(CryptoPP::GCM<CryptoPP::AES>::Encryption());
		default:
			lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
		}
	case BlockCipher::BLOWFISH:
		switch (cm) {
		case CipherMode::CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::Blowfish>::Encryption>(CryptoPP::CCM<CryptoPP::Blowfish>::Encryption());
		case CipherMode::CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption>(CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Encryption());
		case CipherMode::CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Encryption>(CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Encryption());
		case CipherMode::CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Encryption>(CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Encryption());
		case CipherMode::EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::Blowfish>::Encryption>(CryptoPP::EAX<CryptoPP::Blowfish>::Encryption());
		case CipherMode::GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::Blowfish>::Encryption>(CryptoPP::GCM<CryptoPP::Blowfish>::Encryption());
		default:
			lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
		}
	case BlockCipher::CAMELLIA:
		switch (cm) {
		case CipherMode::CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::Camellia>::Encryption>(CryptoPP::CCM<CryptoPP::Camellia>::Encryption());
		case CipherMode::CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::Camellia>::Encryption>(CryptoPP::CBC_Mode<CryptoPP::Camellia>::Encryption());
		case CipherMode::CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::Camellia>::Encryption>(CryptoPP::CFB_Mode<CryptoPP::Camellia>::Encryption());
		case CipherMode::CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::Camellia>::Encryption>(CryptoPP::CTR_Mode<CryptoPP::Camellia>::Encryption());
		case CipherMode::EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::Camellia>::Encryption>(CryptoPP::EAX<CryptoPP::Camellia>::Encryption());
		case CipherMode::GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::Camellia>::Encryption>(CryptoPP::GCM<CryptoPP::Camellia>::Encryption());
		default:
			lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
		}
	case BlockCipher::CAST6:
		switch (cm) {
		case CipherMode::CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::CAST256>::Encryption>(CryptoPP::CCM<CryptoPP::CAST256>::Encryption());
		case CipherMode::CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::CAST256>::Encryption>(CryptoPP::CBC_Mode<CryptoPP::CAST256>::Encryption());
		case CipherMode::CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::CAST256>::Encryption>(CryptoPP::CFB_Mode<CryptoPP::CAST256>::Encryption());
		case CipherMode::CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::CAST256>::Encryption>(CryptoPP::CTR_Mode<CryptoPP::CAST256>::Encryption());
		case CipherMode::EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::CAST256>::Encryption>(CryptoPP::EAX<CryptoPP::CAST256>::Encryption());
		case CipherMode::GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::CAST256>::Encryption>(CryptoPP::GCM<CryptoPP::CAST256>::Encryption());
		default:
			lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
		}
	default:
		lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
	}
}

std::variant<std::unique_ptr<CryptoPP::CipherModeBase>, std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>> getDecCipher(enum BlockCipher bc, enum CipherMode cm) {
	switch (bc) {
	case BlockCipher::AES:
		switch (cm) {
		case CipherMode::CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::AES>::Decryption>(CryptoPP::CCM<CryptoPP::AES>::Decryption());
		case CipherMode::CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption>(CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption());
		case CipherMode::CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption>(CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption());
		case CipherMode::CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption>(CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption());
		case CipherMode::EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::AES>::Decryption>(CryptoPP::EAX<CryptoPP::AES>::Decryption());
		case CipherMode::GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::AES>::Decryption>(CryptoPP::GCM<CryptoPP::AES>::Decryption());
		default:
			lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
		}
	case BlockCipher::BLOWFISH:
		switch (cm) {
		case CipherMode::CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::Blowfish>::Decryption>(CryptoPP::CCM<CryptoPP::Blowfish>::Decryption());
		case CipherMode::CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Decryption>(CryptoPP::CBC_Mode<CryptoPP::Blowfish>::Decryption());
		case CipherMode::CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Decryption>(CryptoPP::CFB_Mode<CryptoPP::Blowfish>::Decryption());
		case CipherMode::CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Decryption>(CryptoPP::CTR_Mode<CryptoPP::Blowfish>::Decryption());
		case CipherMode::EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::Blowfish>::Decryption>(CryptoPP::EAX<CryptoPP::Blowfish>::Decryption());
		case CipherMode::GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::Blowfish>::Decryption>(CryptoPP::GCM<CryptoPP::Blowfish>::Decryption());
		default:
			lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
		}
	case BlockCipher::CAMELLIA:
		switch (cm) {
		case CipherMode::CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::Camellia>::Decryption>(CryptoPP::CCM<CryptoPP::Camellia>::Decryption());
		case CipherMode::CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::Camellia>::Decryption>(CryptoPP::CBC_Mode<CryptoPP::Camellia>::Decryption());
		case CipherMode::CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::Camellia>::Decryption>(CryptoPP::CFB_Mode<CryptoPP::Camellia>::Decryption());
		case CipherMode::CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::Camellia>::Decryption>(CryptoPP::CTR_Mode<CryptoPP::Camellia>::Decryption());
		case CipherMode::EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::Camellia>::Decryption>(CryptoPP::EAX<CryptoPP::Camellia>::Decryption());
		case CipherMode::GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::Camellia>::Decryption>(CryptoPP::GCM<CryptoPP::Camellia>::Decryption());
		default:
			lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
		}
	case BlockCipher::CAST6:
		switch (cm) {
		case CipherMode::CCM:
			return std::make_unique<CryptoPP::CCM<CryptoPP::CAST256>::Decryption>(CryptoPP::CCM<CryptoPP::CAST256>::Decryption());
		case CipherMode::CBC:
			return std::make_unique<CryptoPP::CBC_Mode<CryptoPP::CAST256>::Decryption>(CryptoPP::CBC_Mode<CryptoPP::CAST256>::Decryption());
		case CipherMode::CFB:
			return std::make_unique<CryptoPP::CFB_Mode<CryptoPP::CAST256>::Decryption>(CryptoPP::CFB_Mode<CryptoPP::CAST256>::Decryption());
		case CipherMode::CTR:
			return std::make_unique<CryptoPP::CTR_Mode<CryptoPP::CAST256>::Decryption>(CryptoPP::CTR_Mode<CryptoPP::CAST256>::Decryption());
		case CipherMode::EAX:
			return std::make_unique<CryptoPP::EAX<CryptoPP::CAST256>::Decryption>(CryptoPP::EAX<CryptoPP::CAST256>::Decryption());
		case CipherMode::GCM:
			return std::make_unique<CryptoPP::GCM<CryptoPP::CAST256>::Decryption>(CryptoPP::GCM<CryptoPP::CAST256>::Decryption());
		default:
			lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
		}
	default:
		lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
	}
}

int getBlockSize(BlockCipher bc) {
	switch (bc) {
	case BlockCipher::AES:
		return CryptoPP::AES::BLOCKSIZE;
	case BlockCipher::BLOWFISH:
		return CryptoPP::Blowfish::BLOCKSIZE;
	case BlockCipher::CAMELLIA:
		return CryptoPP::Camellia::BLOCKSIZE;
	case BlockCipher::CAST6:
		return CryptoPP::CAST256::BLOCKSIZE;
	default:
		lnthrow(std::runtime_error, "Switch case covered all enums but still fell through. This is a major bug.");
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
		lnthrow(std::logic_error, "Key length " + std::to_string(keyLen) + " cannot be used with block cipher " + bcToString(bc));
	}
	this->impl->key = keyPair.first;
	this->impl->iv = keyPair.second;
	this->impl->mode = getEncCipher(bc, cb);
}

void Symmetric::encryptData(const unsigned char* in, size_t inLen, unsigned char* out, size_t outLen) const {
	const auto processCipherModeBase = [this](const unsigned char* in, size_t inLen, unsigned char* out) {
		std::get<std::unique_ptr<CryptoPP::CipherModeBase>>(this->impl->mode)->ProcessData(out, in, inLen);
	};

	const auto processAuthenticatedSymmetricCipherBase = [this](const unsigned char* in, size_t inLen, unsigned char* out) {
		std::get<std::unique_ptr<CryptoPP::AuthenticatedSymmetricCipherBase>>(this->impl->mode)->ProcessData(out, in, inLen);
	};

	if (inLen != outLen) {
		lnthrow(std::logic_error, "inLen (" + std::to_string(inLen) + ") does not equal outLen (" + std::to_string(outLen) + ")");
	}

	if (std::holds_alternative<std::unique_ptr<CryptoPP::CipherModeBase>>(this->impl->mode)) {
		processCipherModeBase(in, inLen, out);
	}
	else {
		processAuthenticatedSymmetricCipherBase(in, inLen, out);
	}
}

static void __encryptFile(std::ifstream& fsIn, std::ofstream& fsOut, const Symmetric* sym) {
	size_t len;
	do {
		unsigned char buf[65536];
		len = fsIn.readsome(reinterpret_cast<char*>(buf), sizeof(buf));
		sym->encryptData(buf, len, buf, len);
		fsOut.write(reinterpret_cast<char*>(buf), len);
	} while (len > 0);
}

void Symmetric::encryptFile(const char* filenameIn, const char* filenameOut) const {
	std::ifstream ifs;
	std::ofstream ofs;

	ifs.open(filenameIn);
	if (!ifs) {
		lnthrow(fs::IOException, std::string("Failed to open input file \"") + filenameIn + "\" (" + std::strerror(errno) + ")");
	}
	ofs.open(filenameOut);
	if (!ofs) {
		lnthrow(fs::IOException, std::string("Failed to open output file \"") + filenameOut + "\" (" + std::strerror(errno) + ")");
	}

	__encryptFile(ifs, ofs, this);

	if (!ifs) {
		lnthrow(fs::IOException, std::string("Input file I/O error: ") + std::strerror(errno));
	}
	if (!ofs) {
		lnthrow(fs::IOException, std::string("Output file I/O error: ") + std::strerror(errno));
	}
}

void Symmetric::encryptFile(const char* filenameInOut) const {
	if (!fs::isDirectory(filenameInOut)) {
		lnthrow(std::runtime_error, std::string("\"") + filenameInOut + "\" is not a file");
	}

	std::ifstream ifs;
	std::pair<std::string, std::ofstream> tmpFile = fs::makeTemp(fs::parentDir(filenameInOut).c_str());

	__encryptFile(ifs, tmpFile.second, this);
	tmpFile.second.close();

	fs::remove(filenameInOut);
	try {
		fs::move(tmpFile.first.c_str(), filenameInOut);
	}
	catch (fs::IOException& e) {
		lnthrow(fs::IOException, std::string("Failed to move temporary file \"") + tmpFile.first + "\" to output \"" + filenameInOut + "\"", e);
	}
}

}

