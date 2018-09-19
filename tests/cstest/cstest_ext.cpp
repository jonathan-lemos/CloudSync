/** @file tests/cstest/cstest_ext.cpp
 * @brief CloudSync testing framework extended functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cstest_ext.hpp"
#include <fstream>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>

namespace CloudSync::Testing{

void createFile(const char* path, void* mem, size_t memLen){
	std::ofstream ofs;
	ofs.open(path);
	if (!ofs){
		throw std::runtime_error("Failed to create file " + std::string(path) + " (" + std::strerror(errno) + ")");
	}

	ofs.write((char*)mem, memLen);
	ofs.close();
	if (!ofs){
		throw std::runtime_error("Failed to write to file " + std::string(path));
	}
}

int cmpFile(const char* file1, const char* file2){
	char c1;
	char c2;
	std::ifstream if1;
	std::ifstream if2;

	if1.open(file1);
	if (!if1){
		throw std::runtime_error("Failed to open file " + std::string(file1) + " (" + std::strerror(errno) + ")");
	}
	if2.open(file2);
	if (!if1){
		throw std::runtime_error("Failed to open file " + std::string(file2) + " (" + std::strerror(errno) + ")");
	}

	do{
		c1 = if1.get();
		c2 = if2.get();
	}while (c1 != EOF && c1 == c2);

	return c1 - c2;
}

int cmpFile(const char* file, void* mem, size_t memLen){
	char c1;
	unsigned char* ucmem = (unsigned char*)mem;
	size_t memPtr = 0;
	std::ifstream ifs;

	ifs.open(file);
	if (!ifs){
		throw std::runtime_error("Failed to open file " + std::string(file) + " (" + std::strerror(errno) + ")");
	}

	for (; memPtr < memLen && (c1 = ifs.get()) == ucmem[memPtr]; memPtr++);

	return c1 - ucmem[memPtr];
}

int cmpFile(void* mem, size_t memLen, const char* file){
	return -cmpFile(file, mem, memLen);
}

bool fileExists(const char* file){
	struct stat st;
	if (stat(file, &st) != 0){
		if (errno != ENOENT && errno != ENOTDIR){
			throw std::runtime_error("Failed to stat " + std::string(file) + " (" + std::strerror(errno) + ")");
		}
		return false;
	}
	return S_ISREG(st.st_mode);
}

static unsigned csRandSeed = 0;

static void csSrand(unsigned seed){
	csRandSeed = seed;
}

static int csRand(){
	csRandSeed = 1103515245 * csRandSeed + 12345;
	return csRandSeed;
}

void fillMemory(void* mem, size_t len){
	unsigned char* ucmem = (unsigned char*)mem;
	for (size_t i = 0; i < len; ++i){
		ucmem[i] = csRand() % ('Z' - 'A') + 'A';
	}
}

void fillMemory(void* mem, size_t len, unsigned seed){
	unsigned char* ucmem = (unsigned char*)mem;
	csSrand(seed);

	for (size_t i = 0; i < len; ++i){
		ucmem[i] = csRand();
	}
}

std::string __makepath(std::vector<std::string> components){
	std::string ret;
	if (components.empty()){
		return "";
	}
	ret = components[0];

	for (size_t i = 1; i < components.size(); ++i){
		if (ret[ret.size() - 1] != '/'){
			ret += '/';
		}
		ret += components[i];
	}

	return ret;
}

struct TestEnvironment::TestEnvironmentImpl{
	std::vector<std::string> files;
	std::vector<std::string> directories;
};

TestEnvironment::TestEnvironment(): impl(std::make_unique<TestEnvironmentImpl>()){}

TestEnvironment::TestEnvironment(TestEnvironment& other){
	swap(*this, other);
}

void swap(TestEnvironment& te1, TestEnvironment& te2){
	std::swap(te1.impl, te2.impl);
}

TestEnvironment& TestEnvironment::operator=(TestEnvironment& other){
	swap(*this, other);
	return *this;
}

TestEnvironment setupBasicEnvironment(const char* basePath){
	TestEnvironment te;
	te.impl->directories.push_back(basePath);


	if (mkdir(basePath, 0755) != 0){
		throw std::runtime_error("Failed to create directory " + std::string(basePath) + '(' + std::strerror(errno) + ')');
	}

	csSrand(0);

	for (int i = 1; i <= 20; ++i){
		unsigned char randData[4096];
		unsigned char randLen = csRand() % sizeof(randData) + 1;

		fillMemory(randData, randLen);
		createFile(MAKE_PATH(basePath, "file" + std::to_string(i)).c_str(), randData, randLen);
	}

	return te;
}

TestEnvironment setupFullEnvironment(const char* basePath){
	TestEnvironment te;
	te.impl->directories.push_back(basePath);

	if (mkdir(basePath, 0755) != 0){
		throw std::runtime_error("Failed to create directory " + std::string(basePath) + '(' + std::strerror(errno) + ')');
	}

	if (mkdir(MAKE_PATH(basePath, "dir1").c_str(), 0755) != 0){
		throw std::runtime_error("Failed to create directory " + MAKE_PATH(basePath, "dir1") + '(' + std::strerror(errno) + ')');
	}
}

}
