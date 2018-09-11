/** @file cstest.hpp
 * @brief CloudSync testing framework.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <vector>
#include <stdexcept>

namespace CloudSync{
namespace Testing{

/**
 * @brief Do not throw this exception directly. Use the TEST_ASSERT() macro.
 * This exception indicates that a test failed.
 */
class FailedAssertion : public std::runtime_error{
public:
	FailedAssertion(const char* str) : std::runtime_error(str){}
};

/**
 * @brief Asserts that a particular condition is true, failing the test if not.
 *
 * @param assertion The condition to test. Anything that's valid in an "if" statement can be used here.
 *
 * @exception FailedAssertion Thrown if the assertion is false.
 */
#define TEST_ASSERT(assertion)\
	if (!(assertion)){\
		throw CloudSync::Testing::FailedAssertion(#assertion);\
	}\
	/* The following line makes sure a semicolon is required. */ \
	(void)0

/**
 * @brief Do not call this function directly. Use the UNIT_TEST macro.
 * This function registers a test with the internal test vector.
 *
 * @param test The test to register.
 * @param name The name of the test.
 */
void __registertest(void(*test)(), const char* name);

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
	__registerdummy(void(*test)(), const char* name){
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
	void str();\
	CloudSync::Testing::__registerdummy __##str(str, #str);\
	void str()

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
