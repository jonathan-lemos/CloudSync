/** @file tests/test_cstest.cpp
 * @brief Tests cstest framework
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cstest/cstest.hpp"
#include <iostream>

UNIT_TEST(PASS_arithmetic1){
	ASSERT(2 + 2 == 4);
}

UNIT_TEST(PASS_arithmetic2){
	ASSERT(2 * 2 == 4);
	ASSERT(2 * 2 * 2 == 8);
	ASSERT(4 >> 1 == 2);
}

UNIT_TEST(FAIL_arithmetic1){
	ASSERT(2 + 1 == 4);
}

UNIT_TEST(FAIL_arithmetic2){
	ASSERT(2 + 2 == 4);
	ASSERT(2 + 1 == 4);
}

UNIT_TEST(PASS_expect1){
	std::cout << "expectme" << std::endl;
	EXPECT("expectme");
}

UNIT_TEST(PASS_expect2){
	std::cout << "testing123" << std::endl;
	std::cout << "expectme" << std::endl;
	EXPECT("expectme");
}

UNIT_TEST(PASS_expect3){
	EXPECT("");
}

UNIT_TEST(PASS_expect4){
	std::cerr << "expecterr" << std::endl;
	EXPECT("expecterr");
}

UNIT_TEST(FAIL_expect1){
	EXPECT("failme");
}

UNIT_TEST(FAIL_expect2){
	std::cout << "failme" << std::endl;
	std::cout << "hunter2" << std::endl;
	EXPECT("failme");
}

UNIT_TEST(FAIL_expect3){
	std::cerr << "expecterr" << std::endl;
	std::cout << "expectout" << std::endl;
	EXPECT("expecterr");
}

UNIT_TEST(PASS_send1){
	std::string s;

	std::cout << "testprompt:";
	SEND("teststr");
	std::cin >> s;
	std::cout << s << std::endl;

	EXPECT("testprompt:teststr");
}

UNIT_TEST(PASS_send2){
	std::string s;

	std::cout << "testprompt:";
	SEND("teststr");
	std::cin >> s;

	EXPECT("testprompt:");
}

UNIT_TEST(FAIL_send1){
	std::string s;

	std::cout << "testprompt:";
	SEND("failme");
	std::cin >> s;

	EXPECT("notastring");
}

UNIT_TEST(FAIL_send2){
	std::string s;
	std::cout << "testprompt:";
	SEND("failme");
	std::cin >> s;

	EXPECT("failme");
}

UNIT_TEST(PASS_test_printf){
	TEST_PRINTF("qqq\n");
	TEST_PRINTF("test printf %d\n", 123);
}

UNIT_TEST(FAIL_segfault){
	int* q = NULL;
	*q = 0xF00BA4;
	(void)__iocapt;
}

int main(int argc, char** argv){
	return EXECUTE_TESTS();
}
