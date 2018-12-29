/** @file tests/test_ext.hpp
 * @brief Common functions for testing.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <memory>
#include <unordered_set>

namespace Testing {

/**
 * @brief Creates a file and fills it with the given data.
 *
 * @param filename The filename of the file to create.
 * @param data The data to fill it with.
 * @param dataLen The length of the data to fill.
 *
 * @exception std::runtime_error Failed to create the file.
 */
void createFile(const char* filename, const void* data, size_t dataLen);

/**
 * @brief Compares a file's contents with a given block of data.
 *
 * @param filename The file's path.
 * @param data A pointer to the data to compare it with.
 * @param dataLen The length of the data to compare.
 *
 * @exception std::runtime_error Failed to open/read the file.
 *
 * @return 0 if the contents are the same.
 * Negative if the first char that doesn't match is less in the file than the data, or if the file is shorter than the data.
 * Positive otherwise.
 */
int compare(const char* filename, const void* data, long dataLen);

/**
 * @brief Compares a file's contents with another file's contents.
 *
 * @param filename The first file.
 * @param otherFilename The second file.
 *
 * @exception std::runtime_error Failed to open/read from one of the files.
 *
 * @return 0 if the contents are the same.
 * Negative if the first char that doesn't match is less in the file than the data, or if the file is shorter than the data.
 * Positive otherwise.
 */
int compare(const char* filename, const char* otherFilename);

/**
 * @brief Returns true if a file exists at the given path.
 *
 * @param filename The file.
 *
 * @return True if a file exists at the path, false if not.
 * Returns false if a directory is at the path.
 */
bool fileExists(const char* filename);

/**
 * @brief Returns true if a directory exists at the given path.
 *
 * @param dirname The directory.
 *
 * @return True if a directory exists at the path, false if not.
 * Returns false if a file is at the path.
 */
bool dirExists(const char* dirname);

/**
 * @brief Fills data with a cyclic 01234567890123... pattern.
 *
 * @param data The data to fill.
 * @param len The length to fill.
 */
void fillData(void* data, size_t len);

/**
 * @brief Creates a test environment filled with files.
 */
class TestEnvironment {
public:
	/**
	 * @brief Constructs a "basic" test environment with the following structure:
	 * basePath (0755)
	 *     test[0-count].txt (0644)
	 *
	 * @param basePath The base path to create the test environment within.
	 * @param count The number of files to create.
	 *
	 * @exception std::runtime_error Failed to create one or more files/directories.
	 */
	static TestEnvironment Basic(const char* basePath, int nFiles = 20, int maxFileLen = 4096);

	/**
	 * @brief Constructs a "full" test environment with the following structure:
	 * basePath
	 *     test[0-countPerDir].txt (0755)
	 *     noaccess.txt (0000)
	 *         dir1 (0755)
	 *             d1[0-countPerDir].txt (0644)
	 *         dir2 (0755)
	 *             d2[0-countPerDir].txt (0644)
	 *         excl (0755)
	 *             ex[0-countPerDir].txt (0644)
	 *         noaccess (0000)
	 *             noacc.txt (0644)
	 *
	 * @param basePath The base path to create the test environment within.
	 * @param countPerDir The number of files to create in each directory.
	 *
	 * @exception std::runtime_error Failed to create one or more files/directories.
	 */
	static TestEnvironment Full(const char* basePath, int nFilesPerDir = 20, int maxFileLen = 4096);

	/**
	 * @brief Move constructor.
	 */
	TestEnvironment(TestEnvironment&& other);

	/**
	 * @brief Deleted copy constructor.
	 */
	TestEnvironment(const TestEnvironment& other) = delete;

	/**
	 * @brief Move assignment operator.
	 */
	TestEnvironment& operator=(TestEnvironment&& other);

	/**
	 * @brief Deleted copy assignment operator.
	 */
	TestEnvironment& operator=(const TestEnvironment& other) = delete;

	/**
	 * @brief Gets a hashset containing all the files in this TestEnvironment.
	 *
	 * @return A const reference to an unordered_set containing all the files in this TestEnvironment.
	 */
	const std::unordered_set<std::string>& getFiles();

	/**
	 * @brief Gets a hashset containing all the directories in this TestEnvironment.
	 *
	 * @return A const reference to an unordered_set containing all the directories in this TestEnvironment.
	 */
	const std::unordered_set<std::string>& getDirs();

	/**
	 * @brief Destructor.
	 * This removes all files/directories created by this TestEnvironment.
	 */
	~TestEnvironment();

private:
	/**
	 * @brief Private constructor.
	 * Do not instantiate TestEnvironment directly, instead use TestEnvironment::Basic() or TestEnvironment::Full()
	 */
	TestEnvironment();

	struct TestEnvironmentImpl;
	std::unique_ptr<TestEnvironmentImpl> impl;
};

}
