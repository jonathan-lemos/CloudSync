/** @file terminal.cpp
 * @brief Terminal modifying functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "terminal.hpp"
#include <iostream>

#include <sys/ioctl.h>
#include <unistd.h>

namespace CloudSync{
namespace Terminal{

const char* Color(ColorValue cv){
	switch (cv){
	case BLACK:
		return "\033[30m";
	case RED:
		return "\033[31m";
	case GREEN:
		return "\033[32m";
	case YELLOW:
		return "\033[33m";
	case BLUE:
		return "\033[34m";
	case MAGENTA:
		return "\033[35m";
	case CYAN:
		return "\033[36m";
	case WHITE:
		return "\033[37m";
	case NORMAL:
		return "\033[0m";
	}
}

const char* BrightColor(ColorValue cv){
	switch (cv){
	case BLACK:
		return "\033[90m";
	case RED:
		return "\033[91m";
	case GREEN:
		return "\033[92m";
	case YELLOW:
		return "\033[93m";
	case BLUE:
		return "\033[94m";
	case MAGENTA:
		return "\033[95m";
	case CYAN:
		return "\033[96m";
	case WHITE:
		return "\033[97m";
	case NORMAL:
		return "\033[0m";
	}
}

void ShowCursor(bool enable){
	std::cout << (enable ? "\033[?25h" : "\033[?25l");
}

size_t GetCols(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

size_t GetRows(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
}

}
}
