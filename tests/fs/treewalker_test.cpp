/** @file test_fileiterator.cpp
 * @brief tests fileiterator
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../../fs/treewalker.hpp"
#include "../../fs/ioexception.hpp"
#include "../test_ext.hpp"
#include <gtest/gtest.h>
#include <regex>

TEST(TreeWalkerTest, MainTest) {
	constexpr const char* tmpPath = "tmpPath";
	TestExt::TestEnvironment te = TestExt::TestEnvironment::Full(tmpPath);
	std::unordered_set<std::string> files(te.getFiles());
	CloudSync::fs::TreeWalker fi(tmpPath);
	const char* current = fi.nextEntry();

	while (current != nullptr) {
		ASSERT_TRUE(files.find(current) != files.end());
		files.erase(files.find(current));
		do {
			try {
				current = fi.nextEntry();
				break;
			}
			catch (CloudSync::fs::IOException& e) {
				std::cerr << e.what() << std::endl;
				continue;
			}
		} while (1);
	}

	for (auto it = files.begin(); it != files.end(); ) {
		if (std::regex_match(*it, std::regex(".*noacc.*"))) {
			files.erase(it);
		}
		else {
			++it;
		}
	}

	std::for_each(files.begin(), files.end(), [](const auto& elem) {
		std::cout << elem << std::endl;
	});

	EXPECT_TRUE(files.size() == 0);
}

TEST(SkipDirectoryTest, MainTest) {
	constexpr const char* tmpPath = "tmpPath";
	TestExt::TestEnvironment te = TestExt::TestEnvironment::Full(tmpPath);
	CloudSync::fs::TreeWalker fi(tmpPath);
	const char* current = fi.nextEntry();
	bool foundD2 = false;

	while (current != nullptr) {
		ASSERT_TRUE(fi.currentDirectory());
		if (std::regex_match(current, std::regex(".*dir2.*"))) {
			EXPECT_TRUE(!foundD2);
			fi.skipDirectory();
			foundD2 = true;
		}
		auto currentOld = current;
		do {
			try {
				current = fi.nextEntry();
			}
			catch (CloudSync::fs::IOException& e) {
				std::cerr << e.what() << std::endl;
			}
		} while (current == currentOld);
	}
}

#ifndef __MAIN_TEST__

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif
