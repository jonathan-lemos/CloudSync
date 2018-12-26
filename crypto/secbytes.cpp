/** @file crypto/secbytes.cpp
 * @brief A secure byte class that automatically wipes contents on desctruction.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "secbytes.hpp"
#include <cryptopp/secblock.h>
#include <cstring>
#include <string>

struct SecBytes::SecBytesImpl {
	CryptoPP::SecByteBlock s;
};

SecBytes::SecBytes(): impl(std::make_unique<SecBytesImpl>()) {}

SecBytes::SecBytes(size_t capacity): impl(std::make_unique<SecBytesImpl>()) {
	this->impl->s.resize(capacity);
}

SecBytes::SecBytes(const void* data, size_t data_len): impl(std::make_unique<SecBytesImpl>()) {
	std::memcpy(&(this->impl->s[0]), data, data_len);
}

SecBytes::SecBytes(const char* str): impl(std::make_unique<SecBytesImpl>()) {
	std::memcpy(&(this->impl->s[0]), str, std::strlen(str));
}

SecBytes::SecBytes(SecBytes&& other) {
	this->impl = std::move(other.impl);
}

SecBytes::SecBytes(const SecBytes& other): impl(std::make_unique<SecBytesImpl>()) {
	this->impl->s = other.impl->s;
}

SecBytes::~SecBytes() = default;

unsigned char* SecBytes::data() const {
	return &(this->impl->s[0]);
}

size_t SecBytes::size() const {
	return this->impl->s.size();
}

void SecBytes::resize(size_t capacity) {
	this->impl->s.resize(capacity);
}

SecBytes& SecBytes::operator=(SecBytes&& other) {
	this->impl = std::move(other.impl);
	return *this;
}

SecBytes& SecBytes::operator=(const SecBytes& other) {
	this->impl->s = other.impl->s;
	return *this;
}

unsigned char& SecBytes::operator[](size_t index) const {
	if (index >= this->size()) {
		throw std::out_of_range(std::string("Size = ") + std::to_string(this->size()) + ". Index = " + std::to_string(index) + ".");
	}
	return this->impl->s[index];
}

SecBytes SecBytes::operator+(const SecBytes& other) const {
	SecBytes ret(*this);
	ret += other;
	return ret;
}

SecBytes& SecBytes::operator+=(const SecBytes& other) {
	this->impl->s += other.impl->s;
	return *this;
}

bool SecBytes::operator==(const SecBytes& other) const {
	return this->impl->s == other.impl->s;
}

bool SecBytes::operator!=(const SecBytes& other) const {
	return this->impl->s != other.impl->s;
}
