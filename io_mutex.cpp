/** @file io_mutex.cpp
 * @brief Global mutex for IO thread-safety.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <mutex>

namespace CloudSync{
namespace IO{

static std::mutex io_mutex;

void lock(){
	io_mutex.lock();
}

void unlock(){
	io_mutex.unlock();
}

}
}
