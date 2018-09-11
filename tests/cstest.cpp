/** @file cstest.cpp
 * @brief CloudSync testing framework.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cstest.hpp"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>

#include <signal.h>
#include <setjmp.h>

namespace CloudSync{
namespace Testing{

/**
 * @brief Stores a location in the program that can be longjmp()'d to.
 */
static jmp_buf s_jmpbuf;

/**
 * @brief Stores the last thrown signal that was caught through sig_longjmp()
 */
static volatile sig_atomic_t s_signo;

/**
 * @brief Signal handler.
 * This function is called with SIGINT, SIGABRT, or SIGSEGV is thrown.
 * This function cannot do much, because all functions called from signal handlers need to be re-entrant.
 * Unfortunately, I/O functions such as std::cout are not re-entrant.
 * Fortunately, longjmp is re-entrant, so it can be safely called from within our signal handler.
 */
static void sig_longjmp(int signo){
	s_signo = signo;
	longjmp(s_jmpbuf, signo);
}

/**
 * @brief Returns a string representation of the last thrown signal.
 *
 * @return A string representation of the last thrown signal.
 */
static const char* signal_tostring(){
	switch (s_signo){
		case SIGINT:
			return "Interrupt signal";
		case SIGABRT:
			return "Abort signal";
		case SIGSEGV:
			return "Segmentation fault";
		default:
			return "Unknown signal";
	}
}

/**
 * @brief Exits the program if a fatal signal is received.
 */
static void handle_signal(){
	switch (s_signo){
	case SIGINT:
	case SIGABRT:
		std::exit(1);
		break;
	}
}

/**
 * @brief Sets up the sig_longjmp() function so it handles the signals we want to capture.
 */
static void setup_signal_handler(){
	struct sigaction sa;
	sa.sa_handler = sig_longjmp;
	sigfillset(&(sa.sa_mask));
	sa.sa_flags = SA_RESTART;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);

	if (setjmp(s_jmpbuf)){
		std::cerr << "A setjmp() location was not specified, so the program is terminating." << std::endl;
		std::exit(1);
	}
}

/**
 * @brief Prints the results of the testing.
 *
 * @param __testvec The test vector retrieved through __gettestvec()
 * @param __failvec The tests that failed.
 */
static void print_results(std::vector<std::pair<void(*)(), const char*>>& __testvec, std::vector<std::pair<size_t, const char*>>& __failvec){
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
std::vector<std::pair<void(*)(), const char*>>& __gettestvec(){
	static std::vector<std::pair<void(*)(), const char*>> __testvec;
	return __testvec;
}

void __registertest(void(*test)(), const char* name){
	__gettestvec().push_back(std::make_pair(test, name));
}

int __executetests(){
	size_t i = 0;
	std::vector<std::pair<void(*)(), const char*>>& __testvec = __gettestvec();
	std::vector<std::pair<size_t, const char*>> __failvec;

	// Setup our signal handler
	setup_signal_handler();
	// Set the longjmp location for the signal handler.
	if (setjmp(s_jmpbuf)){
		// If a signal is caught, execution will continue here.
		handle_signal();
		// If the program does not exit.
		__failvec.push_back(std::make_pair(i, __testvec[i].second));
		std::cout << "Crashed (" << signal_tostring() << ")" << std::endl;
		i++;
	}

	for (i = 0; i < __testvec.size(); ++i){
		std::cout << "Test " << i + 1 << " (" << __testvec[i].second << ")...";
		try{
			__testvec[i].first();
			std::cout << "Passed";
		}
		catch (FailedAssertion& e){
			__failvec.push_back(std::make_pair(i, __testvec[i].second));
			std::cout << "Failed";
		}
		std::cout << std::endl;
	}

	print_results(__testvec, __failvec);

	return __failvec.size();
}

}
}
