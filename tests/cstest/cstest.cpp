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

#include <signal.h>
#include <setjmp.h>

namespace CloudSync{
namespace Testing{

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
	std::string q = __iocapt.getLine();
	if (std::strcmp(q.c_str(), str) != 0){
		throw new ExpectationException(str, q.c_str());
	}
}

void __registertest(void(*test)(IOCapturer&), const char* name){
	__gettestvec().push_back(std::make_pair(test, name));
}

int __executetests(int argc, char** argv, IOCapturer& __iocapt){
	size_t i = 0;
	std::vector<std::pair<void(*)(IOCapturer&), const char*>>& __testvec = __gettestvec();
	std::vector<std::pair<size_t, const char*>> __failvec;

	(void)argc;
	(void)argv;

	// Setup our signal handler
	signalHandler{
		defaultHandler();
		// If the program does not exit.
		__failvec.push_back(std::make_pair(i, __testvec[i].second));
		std::cout << "Crashed (" << signalToString(getLastSignal()) << ")" << std::endl;
		i++;
	}

	for (; i < __testvec.size(); ++i){
		std::cout << "Test " << i + 1 << " (" << __testvec[i].second << ")...";
		try{
			__testvec[i].first(__iocapt);
			std::cout << "Passed";
		}
		catch (AssertionException& e){
			__failvec.push_back(std::make_pair(i, __testvec[i].second));
			std::cout << "Failed (" << e.assertion << ")";
		}
		catch (ExpectationException& e){
			std::cout << "Failed (" << e.expected << "!=" << e.actual;
		}
		std::cout << std::endl;
	}

	print_results(__testvec, __failvec);

	return __failvec.size();
}

}
}
