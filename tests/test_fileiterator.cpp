/** @file test_fileiterator.hpp
 * @brief tests fileiterator
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../fileiterator.hpp"

#include "simpletest/simpletest.hpp"
#include "simpletest/simpletest_ext.hpp"
#include <algorithm>
#include <vector>

using namespace simpletest;

UNIT_TEST(test_fi){
	THROW_INSTANTLY_ON_SIGNAL();

	constexpr const char* path = "path";
	TestEnvironment te;
	const char* buf;

	te.setupFullEnvironment(path);

	std::vector<std::string> files(te.getFiles());
	CloudSync::FileIterator fi(path);

	while ((buf = fi.nextEntry()) != nullptr){
		std::vector<std::string>::iterator it = std::find(files.begin(), files.end(), buf);
		ASSERT(it != files.end());

		files.erase(it);
	}
}

int main(int argc, char** argv){
	return EXECUTE_TESTS();
}
