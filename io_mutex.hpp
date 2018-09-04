/** @file io_mutex.hpp
 * @brief Mutex for printing synchronization between threads.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <mutex>

namespace CloudSync{
	std::mutex io_mutex;
}
