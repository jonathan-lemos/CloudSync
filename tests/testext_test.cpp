/** @file tests/testext_test.cpp
 * @brief Tests testext.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "test_ext.hpp"
#include <cstring>
#include <fstream>
#include <ftw.h>
#include <gtest/gtest.h>
#include <regex>
#include <unistd.h>

constexpr const char* testFname1 = "test1.txt";
constexpr const char* testFname2 = "test2.txt";
constexpr const char* testDir = "testdir";

static int ftw_rm(const char* fpath, const struct stat* st, int typeflag, struct FTW* ftwbuf) {
	(void)st; (void)typeflag; (void)ftwbuf;
	chmod(fpath, 0755);
	return remove(fpath);
}

class TestExtTest : public testing::Test {
protected:
	TestExtTest() {}
	virtual ~TestExtTest() {}

	virtual void SetUp() override {}

	virtual void TearDown() override {
		std::remove(testFname1);
		std::remove(testFname2);
		nftw(testDir, ftw_rm, 64, FTW_DEPTH | FTW_PHYS);
	}
};

TEST_F(TestExtTest, CreateFileTest) {
	constexpr const char* data = "test";
	TestExt::createFile(testFname1, data, std::strlen(data));

	std::ifstream ifs(testFname1);
	std::string s;
	ifs >> s;
	ifs.close();

	EXPECT_TRUE(s == data);
}

TEST_F(TestExtTest, CompareTest1) {
	constexpr const char* dataGt1 = "t";
	constexpr const char* dataGt2 = "tess";
	constexpr const char* dataGt3 = "szzz";
	constexpr const char* data = "test";
	constexpr const char* dataLt1 = "testa";
	constexpr const char* dataLt2 = "tesu";
	constexpr const char* dataLt3 = "uzzz";

	std::ofstream ofs(testFname1);
	ofs << data;
	ofs.close();

	EXPECT_TRUE(TestExt::compare(testFname1, dataGt1, std::strlen(dataGt1)) > 0);
	EXPECT_TRUE(TestExt::compare(testFname1, dataGt2, std::strlen(dataGt2)) > 0);
	EXPECT_TRUE(TestExt::compare(testFname1, dataGt3, std::strlen(dataGt3)) > 0);
	EXPECT_TRUE(TestExt::compare(testFname1, data, std::strlen(data)) == 0);
	EXPECT_TRUE(TestExt::compare(testFname1, dataLt1, std::strlen(dataLt1)) < 0);
	EXPECT_TRUE(TestExt::compare(testFname1, dataLt2, std::strlen(dataLt2)) < 0);
	EXPECT_TRUE(TestExt::compare(testFname1, dataLt3, std::strlen(dataLt3)) < 0);
}

TEST_F(TestExtTest, CompareTest2) {
	constexpr const char* dataGt1 = "t";
	constexpr const char* dataGt2 = "tess";
	constexpr const char* dataGt3 = "szzz";
	constexpr const char* data = "test";
	constexpr const char* dataLt1 = "testa";
	constexpr const char* dataLt2 = "tesu";
	constexpr const char* dataLt3 = "uzzz";

	std::ofstream ofs;
	ofs.open(testFname1);
	ofs << data;
	ofs.close();

	ofs.open(testFname2);
	ofs << dataGt1;
	ofs.close();
	EXPECT_TRUE(TestExt::compare(testFname1, testFname2) > 0);

	ofs.open(testFname2);
	ofs << dataGt2;
	ofs.close();
	EXPECT_TRUE(TestExt::compare(testFname1, testFname2) > 0);

	ofs.open(testFname2);
	ofs << dataGt3;
	ofs.close();
	EXPECT_TRUE(TestExt::compare(testFname1, testFname2) > 0);

	ofs.open(testFname2);
	ofs << data;
	ofs.close();
	EXPECT_TRUE(TestExt::compare(testFname1, testFname2) == 0);

	ofs.open(testFname2);
	ofs << dataLt1;
	ofs.close();
	EXPECT_TRUE(TestExt::compare(testFname1, testFname2) < 0);

	ofs.open(testFname2);
	ofs << dataLt2;
	ofs.close();
	EXPECT_TRUE(TestExt::compare(testFname1, testFname2) < 0);

	ofs.open(testFname2);
	ofs << dataLt3;
	ofs.close();
	EXPECT_TRUE(TestExt::compare(testFname1, testFname2) < 0);
}

TEST_F(TestExtTest, FileExistsTest) {
	std::remove(testFname1);
	EXPECT_TRUE(!TestExt::fileExists(testFname1));

	std::ofstream ofs(testFname1);
	ofs << "test";
	ofs.close();

	EXPECT_TRUE(TestExt::fileExists(testFname1));
}

TEST_F(TestExtTest, DirExistsTest) {
	rmdir(testDir);
	EXPECT_TRUE(!TestExt::dirExists(testDir));
	EXPECT_TRUE(TestExt::dirExists("."));

	mkdir(testDir, 0755);

	EXPECT_TRUE(TestExt::dirExists(testDir));
}

TEST_F(TestExtTest, FillDataTest) {
	char data[16];
	constexpr const char* dataExpect = "0123456789012345";

	TestExt::fillData(data, sizeof(data));

	EXPECT_TRUE(std::memcmp(data, dataExpect, sizeof(data)) == 0);
}

TEST_F(TestExtTest, TestEnvironmentBasicTest) {
	std::unordered_set<std::string> dirs;
	std::unordered_set<std::string> files;
	{
		TestExt::TestEnvironment te(TestExt::TestEnvironment::Basic(testDir));
		dirs = std::unordered_set(te.getDirs());
		files = std::unordered_set(te.getFiles());

		for (auto s : dirs) {
			EXPECT_TRUE(TestExt::dirExists(s.c_str()));
		}
		for (auto s : files) {
			EXPECT_TRUE(TestExt::fileExists(s.c_str()));
		}
	}
	for (auto s : dirs) {
		EXPECT_TRUE(!TestExt::dirExists(s.c_str()));
	}
	for (auto s : files) {
		EXPECT_TRUE(!TestExt::fileExists(s.c_str()));
	}
}

TEST_F(TestExtTest, TestEnvironmentFullTest) {
	std::unordered_set<std::string> dirs;
	std::unordered_set<std::string> files;
	{
		TestExt::TestEnvironment te = TestExt::TestEnvironment::Full(testDir);
		dirs = std::unordered_set(te.getDirs());
		files = std::unordered_set(te.getFiles());

		for (auto s : dirs) {
			if (std::regex_match(s, std::regex(".*noaccess.*"))) {
				continue;
			}
			EXPECT_TRUE(TestExt::dirExists(s.c_str()));
		}
		for (auto s : files) {
			if (std::regex_match(s, std::regex(".*noaccess.*"))) {
				continue;
			}
			EXPECT_TRUE(TestExt::fileExists(s.c_str()));
		}
	}
	for (auto s : dirs) {
		if (std::regex_match(s, std::regex(".*noaccess.*"))) {
			continue;
		}
		EXPECT_TRUE(!TestExt::dirExists(s.c_str()));
	}
	for (auto s : files) {
		if (std::regex_match(s, std::regex(".*noaccess.*"))) {
			continue;
		}
		EXPECT_TRUE(!TestExt::fileExists(s.c_str()));
	}
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
