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

constexpr const char* const header = "CF\n";
constexpr const char* const key1 = "key1";
constexpr const char* const key2 = "key2";
const std::vector<unsigned char> data1 = { 'd', 'a', 't', 'a' };
const std::vector<unsigned char> data2 = { '\0', '\n', '\b', 255, 0x1, 0x2 };
const size_t data1len = data1.size();
const size_t data2len = data2.size();

std::vector<unsigned char> makeSampleData(const std::vector<std::pair<std::string, std::vector<unsigned char>>>& pairs) {
	std::vector<unsigned char> ret;
	ret.insert(ret.end(), header, header + std::strlen(header));
	std::for_each(pairs.begin(), pairs.end(), [&ret](const auto& elem) {
		const size_t len = elem.second.size();
		const unsigned char* const lenPtr = reinterpret_cast<const unsigned char*>(&len);
		ret.insert(ret.end(), elem.first.c_str(), elem.first.c_str() + elem.first.length() + 1);
		ret.insert(ret.end(), lenPtr, lenPtr + sizeof(len));
		ret.insert(ret.end(), elem.second.begin(), elem.second.end());
	});
	return ret;
}

const std::vector sampleData = makeSampleData({
	std::make_pair(key1, data1),
	std::make_pair(key2, data2)
});

class ConfigFileTest : public testing::Test {
protected:
	ConfigFileTest() {}

	virtual ~ConfigFileTest() {}

	virtual void SetUp() override {}

	virtual void TearDown() override {
		std::remove(testFname);
	}

private:
	std::vector<unsigned char> __sampleData;
};

TEST_F(ConfigFileTest, WriteTest) {
	{
		CloudSync::ConfigFile file(testFname);
		file.writeEntry(key2, data2.data(), data2.size());
		file.writeEntry(key1, data1);
	}
	EXPECT_TRUE(TestExt::compare(testFname, &(sampleData[0]), sampleData.size()) == 0);
}

TEST_F(ConfigFileTest, ReadTest) {
	std::ofstream ofs(testFname);
	ofs.write(reinterpret_cast<const char*>(&(sampleData[0])), sampleData.size());
	ofs.close();

	CloudSync::ConfigFile cf(testFname);
	auto res = cf.readEntry(key1);
	EXPECT_TRUE(res.has_value());
	EXPECT_TRUE(res.value().get() == data1);
	res = cf.readEntry(key2);
	EXPECT_TRUE(res.has_value());
	EXPECT_TRUE(res.value().get() == data2);
	res = cf.readEntry("noex");
	EXPECT_TRUE(!res.has_value());
}

TEST_F(ConfigFileTest, MultiTest) {
	std::vector<std::string> keys;
	std::vector<unsigned char> data;

	std::ofstream ofs(testFname);
	ofs.write(reinterpret_cast<const char*>(&(sampleData[0])), sampleData.size());
	ofs.close();

	{
		CloudSync::ConfigFile cf(testFname);
		EXPECT_TRUE(!cf.removeEntry("noex"));
		EXPECT_TRUE(cf.removeEntry("key1"));

		keys = cf.getKeys();

		EXPECT_TRUE(keys.size() == 1);
		EXPECT_TRUE(keys[0] == "key2");

		auto res = cf.readEntry(keys[0].c_str());

		EXPECT_TRUE(res.has_value());
		EXPECT_TRUE(res.value().get() == data2);

		cf.flush();

		data = makeSampleData({ std::make_pair(key2, data2) });
		EXPECT_TRUE(TestExt::compare(testFname, data) == 0);

		cf.writeEntry(key1, data1);
		cf.writeEntry(key1, data2);
	}
	data = makeSampleData({ std::make_pair(key1, data2), std::make_pair(key2, data2) });
	EXPECT_TRUE(TestExt::compare(testFname, data) == 0);
}

TEST_F(ConfigFileTest, EmptyTest) {
	constexpr const unsigned char data[] = { 'C', 'F', '\n' };
	{
		CloudSync::ConfigFile cf(testFname);
	}
	EXPECT_TRUE(TestExt::compare(testFname, data, sizeof(data)) == 0);
	{
		CloudSync::ConfigFile cf(testFname);
		EXPECT_TRUE(cf.getKeys().size() == 0);
	}
}

#ifndef __MAIN_TEST__

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

#endif
