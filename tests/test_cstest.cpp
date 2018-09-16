/** @file test_cstest.cpp
 * @brief Tests cstest framework
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cstest/cstest.hpp"
#include <iostream>

UNIT_TEST(test_arithmetic_pass){
	ASSERT(2 + 2 == 4);
}

UNIT_TEST(test_arithmetic_fail){
	ASSERT(2 + 1 == 4);
}

UNIT_TEST(test_expect_pass1){
	std::cout << "expectme" << std::endl;
	EXPECT("expectme");
}

UNIT_TEST(test_expect_pass2){
	EXPECT("");
}

UNIT_TEST(test_expect_fail1){
	EXPECT("failme");
}

UNIT_TEST(test_expect_fail2){
	std::cout << "failme" << std::endl;
	std::cout << "hunter2" << std::endl;
	EXPECT("failme");
}

UNIT_TEST(test_send_pass){
	std::string s;

	std::cout << "testprompt:";
	SEND("teststr");
	std::cin >> s;
	std::cout << s << std::endl;

	EXPECT(s.c_str());
}

UNIT_TEST(test_send_fail){
	std::string s;

	std::cout << "testprompt:";
	SEND("teststr");
	std::cin >> s;

	EXPECT("testprompt:");
}

int main(int argc, char** argv){
	return EXECUTE_TESTS();
}
