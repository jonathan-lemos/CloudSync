/** @file terminal.hpp
 * @brief Terminal modifying functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "attribute.hpp"
#include <cstddef>

namespace CloudSync{
namespace Terminal{

/**
 * @brief A color value to be passed to Color()
 */
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

/**
 * @brief A stream modifier that sets all text printed afterwards to the corresponding color.
 * If stdout or stderr does not point to a tty, this function is a no-op.
 *
 * @param cv The color to set.
 *
 * @return Do not use the return value as its type is not guaranteed. Only use it as a stream modifier like below:<br>
 * ```C++
 * using namespace CloudSync::Terminal;
 * std::cout << Color(RED) << "red" << Color(NORMAL) << std::endl;
 * ```
 */
const char* CS_CONST Color(ColorValue cv);

/**
 * @brief A stream modifier that sets inverts the background color of all text printed afterwards.
 * If stdout or stderr does not point to a tty, this function is a no-op.
 *
 * @param True to invert the following text, false to turn off invert.
 *
 * @return Do not use the return value as its type is not guaranteed. Only use it as a stream modifier like below:<br>
 * ```C++
 * using namespace CloudSync::Terminal
 * std::cout << Invert(true) << "inverted" << Invert(false) << std::endl;
 * ```
 */
const char* CS_CONST Invert(bool enabled);

/**
 * @brief Shows or hides the terminal cursor.
 * If stdout or stderr does not point to a tty, this function is a no-op.
 *
 * @param enable True to show the mouse cursor, false to hide it.
 */
void ShowCursor(bool enabled);

/**
 * @brief Moves the cursor to the coordinate {x, y}
 * The coordinate {0, 0} points to the top-left of the screen.
 * If stdout or stderr does not point to a tty, this function is a no-op.
 *
 * @param row The row to set.
 * @param col The column to set.
 */
void MoveCursor(size_t row, size_t col);

/**
 * @brief Gets the number of columns in the current terminal window.
 */
size_t GetCols();

/**
 * @brief Gets the number of rows in the current terminal window.
 */
size_t GetRows();

#define KEY_UP        (-2)
#define KEY_DOWN      (-3)
#define KEY_RIGHT     (-4)
#define KEY_LEFT      (-5)
#define KEY_ENTER     ('\n')
#define KEY_BACKSPACE ('\x7f')

/**
 * @brief Gets a raw keypress from the terminal.
 *
 * @return The key pressed.
 * KEY_ENTER corresponds to enter.
 * KEY_BACKSPACE corresponds to backspace.
 * KEY_UP, KEY_DOWN, KEY_LEFT, and KEY_RIGHT correspond to their respective arrow keys.
 */
int getch();

}
}
