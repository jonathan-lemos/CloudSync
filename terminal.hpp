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
	BRIGHT_BLACK,
	BRIGHT_RED,
	BRIGHT_GREEN,
	BRIGHT_YELLOW,
	BRIGHT_BLUE,
	BRIGHT_MAGENTA,
	BRIGHT_CYAN,
	BRIGHT_WHITE,
	NORMAL
};

const char* Color(ColorValue cv);
const char* Invert(bool enabled);
void ShowCursor(bool enabled);
void MoveCursor(size_t x, size_t y);
size_t GetCols();
size_t GetRows();

#define KEY_UP        (-2)
#define KEY_DOWN      (-3)
#define KEY_RIGHT     (-4)
#define KEY_LEFT      (-5)
#define KEY_ENTER     ('\n')
#define KEY_BACKSPACE ('\x7f')
int getch();

}
}
