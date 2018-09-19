/** @file tests/cstest/cstest_signal.hpp
 * @brief CloudSync testing framework signal handler.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_CSTEST_SIGNAL_HPP
#define __CS_CSTEST_SIGNAL_HPP

#include <setjmp.h>
#include <signal.h>

namespace CloudSync{
namespace Testing{

/**
 * @brief Do not instantiate.
 * This class makes sure that the signal handler is unregistered on function exit.
 */
class __signalhandler{
public:
	__signalhandler();
	~__signalhandler();


	static jmp_buf s_jmpbuf;
	static volatile sig_atomic_t s_signo;
private:
	static int instanceCount;
};

sig_atomic_t getLastSignal();
const char* signalToString(sig_atomic_t signo);
void defaultHandler(sig_atomic_t signo);

/**
 * @brief Define a signal handler like below:<br>
 * ```C++
 * signalHandler{
 *     //your code here
 * }
 * ```
 * <br>
 * When a signal is thrown, execution will begin at the top of this block and continue until the end of the function it is in.
 */
#define signalHandler __signalhandler __sighand; if (setjmp(__signalhandler::s_jmpbuf))

}
}

#endif
