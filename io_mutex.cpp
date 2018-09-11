/** @file io_mutex.hpp
 * @brief Global mutex for IO thread-safety.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "io_mutex.hpp"

namespace CloudSync{
namespace IO{

std::mutex io_mutex;

}
}
