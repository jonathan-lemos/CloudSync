/** @file cstest_signal.cpp
 * @brief CloudSync testing framework signal handler.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

#include "cstest_signal.hpp"

namespace CloudSync{
namespace Testing{

static void handler(int signo){
	__signalhandler::s_signo = signo;
	longjmp(__signalhandler::s_jmpbuf, signo);
}

__signalhandler::__signalhandler(){
	struct sigaction sa;

	if (instancecount > 0){
		throw new std::logic_error("Cannot have more than one instance of signal handler at a time");
	}
	instancecount++;

	sa.sa_handler = handler;
	sigfillset(&(sa.sa_mask));
	sa.sa_flags = SA_RESTART;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);

	if (setjmp(__signalhandler::s_jmpbuf)){
		std::cerr << "A signalhandler{} location was not specified, so the program is terminating." << std::endl;
		std::exit(1);
	}
}

__signalhandler::~__signalhandler(){
	struct sigaction sa;

	instancecount--;

	sa.sa_handler = SIG_DFL;
	sigfillset(&(sa.sa_mask));
	sa.sa_flags = SA_RESTART;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGABRT, &sa, NULL);
	sigaction(SIGSEGV, &sa, NULL);
}

sig_atomic_t lastSignal(){
	return __signalhandler::s_signo;
}

const char* signalToString(sig_atomic_t signo){
	switch (signo){
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

void defaultHandler(sig_atomic_t signo){
	switch (__signalhandler::s_signo){
	case SIGABRT:
	case SIGINT:
		std::cout << "Exiting program (" << signalToString(signo) << std::endl;
		std::exit(1);
	}
}

}
}
