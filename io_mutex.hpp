/** @file io_mutex.hpp
 * @brief Global mutex for IO thread-safety.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_IO_MUTEX_HPP
#define __CS_IO_MUTEX_HPP

#include <mutex>

namespace CloudSync{
namespace IO{

/**
 * @brief Lock this mutex when using I/O functions to make sure other threads don't write at the same time.
 */
extern std::mutex io_mutex;

}
}

#endif
