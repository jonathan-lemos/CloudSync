/** @file terminal.hpp
 * @brief Terminal modifying functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <cstddef>

namespace CloudSync{
namespace Terminal{

	enum ColorValue{
		BLACK,
		RED,
		GREEN,
		YELLOW,
		BLUE,
		MAGENTA,
		CYAN,
		WHITE,
		NORMAL
	};

	const char* Color(ColorValue cv);
	const char* BrightColor(ColorValue cv);
	void ShowCursor(bool enabled);
	size_t GetCols();
	size_t GetRows();

}
}
