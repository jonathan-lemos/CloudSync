/** @file test_logger.hpp
 * @brief Tests logger.hpp
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cstest.hpp"
//#include "../logger.hpp"
//#include <iostream>

UNIT_TEST(test1){
	TEST_ASSERT(2 + 1 == 4);
}

UNIT_TEST(test2){
	TEST_ASSERT(2 + 2 == 4);
}

int main(){
	/*
	std::cerr << "LEVEL_INFO" << std::endl;
	Logger::setLevel(LEVEL_INFO);
	LOG(LEVEL_FATAL)   << "Fatal";
	LOG(LEVEL_ERROR)   << "Error";
	LOG(LEVEL_WARNING) << "Warning";
	LOG(LEVEL_DEBUG)   << "Debug";
	LOG(LEVEL_INFO)    << "Info";

	std::cerr << "LEVEL_FATAL" << std::endl;
	Logger::setLevel(LEVEL_FATAL);
	LOG(LEVEL_FATAL)   << "Fatal";
	LOG(LEVEL_ERROR)   << "Error";
	LOG(LEVEL_WARNING) << "Warning";
	LOG(LEVEL_DEBUG)   << "Debug";
	LOG(LEVEL_INFO)    << "Info";
	*/

	return EXECUTE_TESTS();
}
