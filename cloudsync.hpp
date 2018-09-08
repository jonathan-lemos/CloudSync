/** @file cloudsync.hpp
 * @brief Synchronizes a disk folder and cloud folder.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

namespace CloudSync{

class Synchronizer{
	Synchronizer(const char* cfgPath = "~/.cloudsync");
}

}
