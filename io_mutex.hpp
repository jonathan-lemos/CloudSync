/** @file io_mutex.hpp
 * @brief Global mutex for IO thread-safety.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <mutex>

namespace CloudSync{
namespace IO{

	/** Lock this mutex when using I/O functions to make sure other threads don't write at the same time. */
	std::mutex io_mutex;

}
}
