/** @file cstest.hpp
 * @brief CloudSync testing framework.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_CSTEST_HPP
#define __CS_CSTEST_HPP

#include "cstest_iocapturer.hpp"
#include <vector>
#include <stdexcept>

namespace CloudSync{
namespace Testing{

/**
 * @brief Do not throw this exception directly. Use the ASSERT() macro instead.
 * This exception indicates that a test assertion failed.
 */
class FailedAssertion : public std::runtime_error{
public:
	FailedAssertion(const char* assertion);
};

/**
 * @brief Asserts that a particular condition is true, failing the test if not.
 *
 * @param assertion The condition to test. Anything that's valid in an "if" statement can be used here.
 *
 * @exception FailedAssertion Thrown if the assertion is false.
 */
#define ASSERT(assertion)\
	/* silences __iocapt unused warning */\
	(void)__iocapt;\
	if (!(assertion)){\
		throw CloudSync::Testing::FailedAssertion(#assertion);\
	}\
	/* The following line makes sure a semicolon is required. */ \
	(void)0

/**
 * @brief Do not throw this exception directly. Use the EXPECT() macro instead.
 * This exception indicates that an expectation failed.
 */
class FailedExpectation : public std::runtime_error{
public:
	FailedExpectation(const char* expected, const char* actual);
};

/**
 * @brief Expects a particular line on stdout, failing the test if not.
 *
 * @param expectation A const char* containing the text to expect. This does not include the newline.
 *
 * @exception FailedExpectation Thrown if the last line on stdout does not match the expectation.
 */
#define EXPECT(expectation)\
	__expect(expectation, __iocapt)

void __expect(const char* str, CloudSync::Testing::IOCapturer& __iocapt);

/**
 * @brief Expects a particular line on stderr, failing the test if not.
 *
 * @param expectation A const char* containing the text to expect. This does not include the newline.
 *
 * @exception ExpectationFailed Thrown if the last line on stdout does not match the expectation.
 */
#define EXPECT_STDERR(expectation)\
	__expect_stderr(expectation, __iocapt)

void __expect_stderr(const char* str, CloudSync::Testing::IOCapturer& __iocapt);

/**
 * @brief Sends a line to stdin.
 *
 * @param line The line to send.
 */
#define SEND(line)\
	__iocapt.sendToStdin(line)

/**
 * @brief Do not call this function directly. Use the UNIT_TEST macro.
 * This function registers a test with the internal test vector.
 *
 * @param test The test to register.
 * @param name The name of the test.
 */
void __registertest(void(*test)(CloudSync::Testing::IOCapturer& __iocapt), const char* name);

/**
 * @brief Do not instantiate this class directly. Use the UNIT_TEST macro.
 * This class is needed to run the __registertest() function at global scope.
 */
class __registerdummy{
public:
	/**
	 * @brief Dummy function used to call __registertest()
	 *
	 * @param test The test to register.
	 * @param name The name of the test.
	 */
	__registerdummy(void(*test)(CloudSync::Testing::IOCapturer& __iocapt), const char* name){
		__registertest(test, name);
	}
};

/**
 * @brief Instantiate a unit test like below:<br>
 * ```C++
 * UNIT_TEST(your_name_here){
 *     TEST_ASSERT(2 + 2 == 4);
 * }
 * ```
 *
 * Tests can be run with the EXECUTE_TESTS() macro.
 */
#define UNIT_TEST(str)\
	void str(CloudSync::Testing::IOCapturer& __iocapt);\
	CloudSync::Testing::__registerdummy __##str(str, #str);\
	void str(CloudSync::Testing::IOCapturer& __iocapt)

/**
 * @brief Do not call this function directly. Use the EXECUTE_TESTS macro.
 * This function executes all tests registered through the UNIT_TEST macro.
 *
 * @param argc The command-line argument count.
 * @param argv The command-line arguments.
 */
int __executetests(int argc, char** argv);

/**
 * @brief This function executes all tests registered through the UNIT_TEST macro.
 * This must be called within the main() function, and said function must have the following signature:<br>
 * ```C++
 * int main(int argc, char** argv);
 * ```
 * <br>
 * The parameters must be called "argc" and "argv"
 *
 * @return The number of tests that failed.
 */
#define EXECUTE_TESTS() CloudSync::Testing::__executetests(argc, argv)

}
}

#endif
