/** @file terminal.cpp
 * @brief Terminal modifying functions.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "../../terminal.hpp"
#include <iostream>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace CloudSync{
namespace Terminal{

static bool CS_CONST is_tty(){
	return !(!isatty(STDOUT_FILENO) && errno == ENOTTY);
}

const char* CS_CONST Color(ColorValue cv){
	if (!is_tty()){
		return "";
	}

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
	case BRIGHT_BLACK:
		return "\033[90m";
	case BRIGHT_RED:
		return "\033[91m";
	case BRIGHT_GREEN:
		return "\033[92m";
	case BRIGHT_YELLOW:
		return "\033[93m";
	case BRIGHT_BLUE:
		return "\033[94m";
	case BRIGHT_MAGENTA:
		return "\033[95m";
	case BRIGHT_CYAN:
		return "\033[96m";
	case BRIGHT_WHITE:
		return "\033[97m";
	case NORMAL:
		return "\033[0m";
	}
	return nullptr;
}

const char* Invert(bool enabled){
	if (enabled){
		return "\033[7m";
	}
	else{
		return "\033[27m";
	}
}

void ShowCursor(bool enabled){
	std::cout << (enable ? "\033[?25h" : "\033[?25l");
}

void echo(bool enabled) {
	struct termios t;
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag = (t.c_lflag & ~(ECHO)) | (enabled ? ECHO : 0);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void MoveCursor(size_t row, size_t col){
	row++;
	col++;
	std::cout << "\033[" << row << ";" << col << "H";
}

void Clear(){
	std::cout << "\033[2J" << "\033[1;1H";
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

int getch(void){
	struct termios t_old;
	struct termios t_new;
	int c;

	tcgetattr(STDIN_FILENO, &t_old);
	t_new = t_old;
	t_new.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

	do{
		c = getchar();
		if (c == '\033'){
			getchar();
			c = getchar();
			tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
			switch (c){
			case 'A':
				return KEY_UP;
			case 'B':
				return KEY_DOWN;
			case 'C':
				return KEY_RIGHT;
			case 'D':
				return KEY_LEFT;
			}
		}
	}while (c == EOF);
	tcsetattr(STDIN_FILENO, TCSANOW, &t_old);
	return c;
}

}
}
