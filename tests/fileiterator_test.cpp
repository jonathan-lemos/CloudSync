/** @file test_fileiterator.cpp
 * @brief tests fileiterator
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../fileiterator.hpp"
#include "test_ext.hpp"
#include <gtest/gtest.h>

TEST(FileIteratorTest, MainTest) {
	constexpr const char* tmpPath = "tmpPath";
	TestExt::TestEnvironment te = TestExt::TestEnvironment::Full(tmpPath);
	std::unordered_set<std::string> files(te.getFiles());
	CloudSync::FileIterator fi(tmpPath);
	const char* current = fi.nextEntry();
	EXPECT_TRUE(current != nullptr);

	do {
		EXPECT_TRUE(fi.currentDirectory() == tmpPath);
		EXPECT_TRUE(files.find(current) != files.end());
		files.erase(files.find(current));
		current = fi.nextEntry();
	} while (current != nullptr);

	EXPECT_TRUE(files.size() == 0);
}

TEST(SkipDirectoryTest, MainTest) {
	constexpr const char* tmpPath = "tmpPath";
	TestExt::TestEnvironment te = TestExt::TestEnvironment::Full(tmpPath);
	CloudSync::FileIterator fi(tmpPath);
	std::string initialDir;
	const char* current = fi.nextEntry();

	EXPECT_TRUE(current != nullptr);
	initialDir = fi.currentDirectory();
	fi.skipDirectory();
	current = fi.nextEntry();
	EXPECT_TRUE(current != nullptr);

	do {
		EXPECT_TRUE(fi.currentDirectory() != initialDir);
		EXPECT_TRUE(std::string(current).substr(0, initialDir.size()) != initialDir);
		current = fi.nextEntry();
	} while (current);
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
