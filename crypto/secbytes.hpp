/** @file crypto/secbytes.hpp
 * @brief A secure byte class that automatically wipes contents on desctruction.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_CRYPTO_SECBYTES_HPP
#define __CS_CRYPTO_SECBYTES_HPP

#include <cstddef>
#include <memory>

/**
 * @brief A secure byte container class.
 * When this class is destructed, its contents are wiped.
 */
class SecBytes {
public:
	/**
	 * @brief The default constructor for SecBytes.
	 * The contents are empty in this case.
	 */
	SecBytes();

	/**
	 * @brief Constructs a SecBytes with the given capacity.
	 * The initial contents are not initialized.
	 */
	SecBytes(size_t capacity);

	/**
	 * @brief Constructs a SecBytes class out of the given data.
	 *
	 * @param data The data to construct the class out of.
	 * @param data_len The length of the data.
	 */
	SecBytes(const void* data, size_t data_len);

	/**
	 * @brief Constructs a SecBytes out of the given string.
	 *
	 * @param str The string to construct the SecBytes out of.
	 */
	SecBytes(const char* str);

	/**
	 * @brief Move constructor.
	 */
	SecBytes(SecBytes&& other);

	/**
	 * @brief Copy constructor.
	 */
	SecBytes(const SecBytes& other);

	/**
	 * @brief Destructor. Needed so the unique_ptr can work with an incomplete type.
	 */
	~SecBytes();

	/**
	 * @brief Returns a pointer to the first element of this SecBytes, or nullptr if it's empty.
	 */
	unsigned char* data() const;

	/**
	 * @brief Returns the amount of elements in the SecBytes instance.
	 */
	size_t size() const;

	/**
	 * @brief Resizes the SecBytes block.
	 * Any new data allocated is not initialized.
	 */
	void resize(size_t capacity);

	/**
	 * @brief Move assignment operator.
	 */
	SecBytes& operator=(SecBytes&& other);

	/**
	 * @brief Copy assignment operator.
	 */
	SecBytes& operator=(const SecBytes& other);

	/**
	 * @brief Array operator that gets the n'th element of this SecBytes.
	 *
	 * @return A reference to the nth byte of this SecBytes.
	 * @exception std::out_of_range The element you are trying to access is out of range of the array.
	 */
	unsigned char& operator[](size_t index) const;

	/**
	 * @brief Adds two SecBytes classes together, returning a new SecBytes which is a concatenation of their contents.
	 */
	SecBytes operator+(const SecBytes& other) const;

	/**
	 * @brief Adds another SecBytes to this one, concatenating its contents to this one's.
	 */
	SecBytes& operator+=(const SecBytes& other);

	/**
	 * @brief Returns true if two SecBytes classes have the same contents, false if not.
	 */
	bool operator==(const SecBytes& other) const;

private:
	struct SecBytesImpl;
	std::unique_ptr<SecBytesImpl> impl;
};

#endif
