/** @file test_config.cpp
 * @brief tests config
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../config.hpp"
#include "test_ext.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

constexpr const char* testFname = "test.txt";

class ConfigFileTest : public testing::Test {
protected:
	const char* const header = "CF\n";
	const char* const key1 = "key1";
	const char* const key2 = "key2";
	const unsigned char data1[4] = { 'd', 'a', 't', 'a' };
	const unsigned char data2[6] = { '\0', '\n', '\b', 255, 0x1, 0x2 };
	const size_t data1len = sizeof(data1);
	const size_t data2len = sizeof(data2);

	ConfigFileTest() {
		__sampleData.insert(__sampleData.end(), header, header + std::strlen(header));
		__sampleData.insert(__sampleData.end(), key1, key1 + std::strlen(key1));
		__sampleData.insert(__sampleData.end(), &data1len, &data1len + 1);
		__sampleData.insert(__sampleData.end(), std::begin(data1), std::end(data1));
		__sampleData.insert(__sampleData.end(), key2, key2 + std::strlen(key2));
		__sampleData.insert(__sampleData.end(), &data2len, &data2len + 1);
		__sampleData.insert(__sampleData.end(), std::begin(data2), std::end(data2));
	}
	virtual ~ConfigFileTest() {}

	virtual void SetUp() override {}

	virtual void TearDown() override {
		std::remove(testFname);
	}

	const std::vector<unsigned char>& sampleData() const {
		return this->__sampleData;
	}

private:
	std::vector<unsigned char> __sampleData;
};

TEST_F(ConfigFileTest, WriteTest) {
	{
		CloudSync::ConfigFile file(testFname);
		file.writeEntry(key2, (void*)data2, sizeof(data2));
		file.writeEntry(key1, (void*)data1, sizeof(data1));
	}
	EXPECT_TRUE(TestExt::compare(testFname, &(sampleData()[0]), sampleData().size()) == 0);
}

TEST_F(ConfigFileTest, ReadTest) {
	std::ofstream ofs(testFname);
	ofs.write(reinterpret_cast<const char*>(&(sampleData()[0])), sampleData().size());
	ofs.close();

	CloudSync::ConfigFile cf(testFname);
	auto res = cf.readEntry(key1);
	EXPECT_TRUE(res);
	EXPECT_TRUE(std::memcmp(&res.value().get()[0], data1, sizeof(data1)) == 0);
	res = cf.readEntry(key2);
	EXPECT_TRUE(res);
	EXPECT_TRUE(std::memcmp(&res.value().get()[0], data2, sizeof(data2)) == 0);
	res = cf.readEntry("noex");
	EXPECT_TRUE(!res);
}

TEST_F(ConfigFileTest, MultiTest) {
	std::ofstream ofs(testFname);
	ofs.write(reinterpret_cast<const char*>(&(sampleData()[0])), sampleData().size());
	ofs.close();

	CloudSync::ConfigFile cf(testFname);
	EXPECT_TRUE(!cf.removeEntry("noex"));
	EXPECT_TRUE(cf.removeEntry("key1"));

	EXPECT_TRUE(cf.getAllEntries().size() == 1);
}
