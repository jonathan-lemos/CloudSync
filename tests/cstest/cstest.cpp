/** @file cstest.cpp
 * @brief CloudSync testing framework.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cstest.hpp"
#include "cstest_signal.hpp"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <optional>

#include <signal.h>
#include <setjmp.h>

namespace CloudSync{
namespace Testing{

FailedAssertion::FailedAssertion(const char* assertion): std::runtime_error(assertion) {}

FailedExpectation::FailedExpectation(const char* expected, const char* actual): std::runtime_error('\"' + std::string(expected) + "\" != \"" + actual + '\"'){}

/**
 * @brief Prints the results of the testing.
 *
 * @param __testvec The test vector retrieved through __gettestvec()
 * @param __failvec The tests that failed.
 */
static void print_results(std::vector<std::pair<void(*)(IOCapturer&), const char*>>& __testvec, std::vector<std::pair<size_t, const char*>>& __failvec){
	std::cout << std::endl;
	std::cout << "Results:" << std::endl;
	std::cout << __testvec.size() - __failvec.size() << " Passed" << std::endl;
	std::cout << __failvec.size() << " Failed" << std::endl;
	std::cout << std::endl;

	std::cout << "Failed tests:" << std::endl;
	std::for_each(__failvec.begin(), __failvec.end(), [](const auto& elem){
			std::cout << "Test " << elem.first + 1 << " (" << elem.second << ")" << std::endl;
			});
}

/**
 * @brief Returns the test vector.
 * This function is needed so the test vector is initialized before any __registertest() functions are called.
 */
static std::vector<std::pair<void(*)(IOCapturer& __iocapt), const char*>>& __gettestvec(){
	static std::vector<std::pair<void(*)(IOCapturer&), const char*>> __testvec;
	return __testvec;
}

void __expect(const char* str, IOCapturer& __iocapt){
	std::string q = IOCapturer::getLastLine(__iocapt.getStdout());
	if (str != q){
		throw FailedExpectation(str, q.c_str());
	}
}

void __expect_stderr(const char* str, IOCapturer& __iocapt){
	std::string q = IOCapturer::getLastLine(__iocapt.getStderr());
	if (str != q){
		throw FailedExpectation(str, q.c_str());
	}
}

void __registertest(void(*test)(IOCapturer&), const char* name){
	__gettestvec().push_back(std::make_pair(test, name));
}

int __executetests(int argc, char** argv){
	size_t i = 0;
	std::vector<std::pair<void(*)(IOCapturer&), const char*>>& __testvec = __gettestvec();
	std::vector<std::pair<size_t, const char*>> __failvec;
	std::optional<IOCapturer> __iocapt = std::nullopt;

	(void)argc;
	(void)argv;

	// Setup our signal handler
	signalHandler{
		__iocapt.reset();
		defaultHandler(getLastSignal());
		// If the program does not exit.
		__failvec.push_back(std::make_pair(i, __testvec[i].second));
		std::cout << "Crashed (" << signalToString(getLastSignal()) << ")" << std::endl;
		i++;
	}

	for (; i < __testvec.size(); ++i){
		std::cout << "Test " << i + 1 << " (" << __testvec[i].second << ")...";
		try{
			__iocapt.emplace();
			__testvec[i].first(__iocapt.value());
			__iocapt.reset();

			std::cout << "Passed";
		}
		catch (FailedAssertion& e){
			__iocapt.reset();
			__failvec.push_back(std::make_pair(i, __testvec[i].second));
			std::cout << "Failed (" << e.what() << ")";
		}
		catch (FailedExpectation& e){
			__iocapt.reset();
			__failvec.push_back(std::make_pair(i, __testvec[i].second));
			std::cout << "Failed (" << e.what() << ")";
		}
		catch (std::exception& e){
			__iocapt.reset();
			__failvec.push_back(std::make_pair(i, __testvec[i].second));
			std::cout << "Internal error (" << e.what() << ")";
		}
		std::cout << std::endl;
	}

	print_results(__testvec, __failvec);

	return __failvec.size();
}

}
}
