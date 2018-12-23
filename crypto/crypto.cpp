/** @file crypto.hpp
 * @brief Handles encryption.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <cryptopp/cryptlib.h>
#include <cryptopp/modes.h>
#include <cryptopp/aes.h>

namespace CloudSync{

CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption encryptor;

}
