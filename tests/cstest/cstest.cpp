/** @file tests/cstest/cstest.cpp
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
#include <iomanip>
#include <cstring>
#include <optional>

#include <signal.h>
#include <setjmp.h>

namespace CloudSync{
namespace Testing{

FailedAssertion::FailedAssertion(const char* assertion): std::runtime_error(assertion) {
}

FailedExpectation::FailedExpectation(const char* expected, const char* actual): std::runtime_error('\"' + std::string(expected) + "\" == \"" + actual + '\"'){
}

template <typename T>
static T CS_CONST nDigits(T x){
	T val = 1;
	T ctr = 0;
	while (val <= x){
		val *= 10;
		ctr++;
	}
	return ctr;
}

/**
 * @brief Prints the results of the testing.
 *
 * @param __testvec The test vector retrieved through __gettestvec()
 * @param __failvec The tests that failed.
 */
static void printResults(size_t __testvec_size, std::vector<std::tuple<size_t, const char*, std::string>>& __failvec){
	size_t maxLen = std::strlen(std::get<const char*>(__failvec[0]));
	std::for_each(__failvec.begin() + 1, __failvec.end(), [&maxLen](const auto& elem){
		if (std::strlen(std::get<const char*>(elem)) > maxLen){
			maxLen = std::strlen(std::get<const char*>(elem));
		}
	});

	std::cout << std::endl;
	std::cout << "Results:" << std::endl;
	std::cout << __testvec_size - __failvec.size() << " Passed" << std::endl;
	std::cout << __failvec.size() << " Failed" << std::endl;
	std::cout << std::endl;

	if (__failvec.size() > 0){
		std::cout << "Failed tests:" << std::endl;
		std::for_each(__failvec.begin(), __failvec.end(), [__testvec_size, maxLen](const auto& elem){
			std::cout << "Test " << std::left << std::setw(nDigits(__testvec_size)) << std::get<size_t>(elem) + 1 << " (" << std::get<const char*>(elem) << ')';
			for (size_t i = 0; i < maxLen - std::strlen(std::get<const char*>(elem)) + 3; ++i){
				std::cout << '.';
			}
			std::cout << '(' << std::get<std::string>(elem) << ')' << std::endl;
		});
	}
	else{
		std::cout << "No failed tests" << std::endl;
	}
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

void __registertest(void(*test)(IOCapturer&), const char* name){
	__gettestvec().push_back(std::make_pair(test, name));
}

/* i love c++ */
static std::vector<std::tuple<size_t, const char*, std::string>> runTests(std::vector<std::pair<void(*)(IOCapturer&), const char*>>& __testvec){
	size_t i = 0;
	size_t maxLen = 0;
	std::vector<std::tuple<size_t, const char*, std::string>> __failvec;
	std::optional<IOCapturer> __iocapt = std::nullopt;

	if (__testvec.size() == 0){
		return {};
	}

	maxLen = std::strlen(__testvec[0].second);
	std::for_each(__testvec.begin() + 1, __testvec.end(), [&maxLen](const auto& elem){
		if (std::strlen(elem.second) > maxLen){
			maxLen = std::strlen(elem.second);
		}
	});

	// Setup our signal handler
	signalHandler{
		__iocapt.reset();
		defaultHandler(getLastSignal());
		// If the program does not exit.
		__failvec.push_back(std::make_tuple(i, __testvec[i].second, std::string("Crashed: ") + signalToString(getLastSignal())));
		std::cout << "Crashed (" << signalToString(getLastSignal()) << ")" << std::endl;
		i++;
	}

	for (; i < __testvec.size(); ++i){
		std::cout << "Test " << std::left << std::setw(nDigits(__testvec.size())) << i + 1 << " (" << __testvec[i].second << ")";
		for (size_t j = 0; j < maxLen - std::strlen(__testvec[i].second) + 3; ++j){
			std::cout << '.';
		}

		try{
			__iocapt.emplace();
			__testvec[i].first(__iocapt.value());
			__iocapt.reset();

			std::cout << "Passed";
		}
		catch (FailedAssertion& e){
			__iocapt.reset();
			__failvec.push_back(std::make_tuple(i, __testvec[i].second, e.what()));
			std::cout << "Failed (" << e.what() << ")";
		}
		catch (FailedExpectation& e){
			__iocapt.reset();
			__failvec.push_back(std::make_tuple(i, __testvec[i].second, e.what()));
			std::cout << "Failed (" << e.what() << ")";
		}
		catch (std::exception& e){
			__iocapt.reset();
			__failvec.push_back(std::make_tuple(i, __testvec[i].second, std::string("Internal error :") + e.what()));
			std::cout << "Internal error (" << e.what() << ")";
		}
		std::cout << std::endl;
	}

	return __failvec;
}

int __executetests(int argc, char** argv){
	std::vector<std::pair<void(*)(IOCapturer&), const char*>>& __testvec = __gettestvec();
	std::vector<std::tuple<size_t, const char*, std::string>> __failvec;
	std::optional<IOCapturer> __iocapt = std::nullopt;

	(void)argc;
	(void)argv;

	__failvec = runTests(__testvec);

	printResults(__testvec.size(), __failvec);

	return __failvec.size();
}

}
}
