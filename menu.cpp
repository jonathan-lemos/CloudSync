/** @file menu.cpp
 * @brief Displays a menu to the user.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "menu.hpp"
#include "terminal.hpp"
#include <iostream>

#include <signal.h>
#include <setjmp.h>

namespace CloudSync{

static jmp_buf s_jmpbuf;

struct Menu::MenuImpl{
	Alignment menuGravity = CENTER;
	Axis axis = VERTICAL;
	std::vector<std::string> entryVec;

	ssize_t DisplayHorizontal();
	ssize_t DisplayVertical();
};

static void on_sigwinch(int signo){
	if (signo != SIGWINCH){
		return;
	}
	longjmp(s_jmpbuf, signo);
}

static void set_sigwinch_handler(){
	struct sigaction sa;
	sa.sa_handler = on_sigwinch;
	sigfillset(&(sa.sa_mask));

	sigaction(SIGWINCH, &sa, NULL);
}

static void remove_sigwinch_handler(){
	struct sigaction sa;
	sa.sa_handler = SIG_DFL;
	sigaction(SIGWINCH, &sa, NULL);
}

static void PrintEntryVertical(std::string entry, size_t row, Menu::Alignment menuGravity){
	size_t nCols = Terminal::GetCols();
	ssize_t startingIndex = 0;

	switch (menuGravity){
	case Menu::LEFT:
		startingIndex = 0;
		break;
	case Menu::CENTER:
		startingIndex = (nCols - entry.size()) / 2;
		break;
	case Menu::RIGHT:
		startingIndex = nCols - entry.size() - 1;
		break;
	}

	if (startingIndex < 0){
		startingIndex = 0;
		entry.resize(nCols - 4);
		entry += "...";
	}

	Terminal::MoveCursor(row, startingIndex);
	std::cout << entry << std::endl;
}

static void PrintBox(int row1, int col1, int row2, int col2){
	int i;
	const char* locale = std::setlocale(LC_ALL, NULL);

	if (row1 > row2){
		int tmp = row1;
		row1 = row2;
		row2 = tmp;
	}
	if (col1 > col2){
		int tmp = col1;
		col1 = col2;
		col2 = tmp;
	}

	std::setlocale(LC_ALL, "");
	Terminal::MoveCursor(row1, col1);
	std::cout <<  "┌";
	for (i = col1 + 1; i <= col2 - 1; ++i){
		std::cout << "─";
	}
	std::cout << "┐";
	for (i = row1 + 1; i <= row2 - 1; ++i){
		Terminal::MoveCursor(i, col1);
		std::cout << "│";
		Terminal::MoveCursor(i, col2);
		std::cout << "│";
	}
	Terminal::MoveCursor(row2, col1);
	std::cout << "└";
	for (i = col1 + 1; i <= col2 - 1; ++i){
		std::cout << "─";
	}
	std::cout << "┘";

	setlocale(LC_ALL, locale);
}

static void PrintMenuVertical(size_t nCols, size_t nRows, size_t vecPtr, std::vector<std::string>& entryVec, Menu::Alignment menuGravity){
	PrintBox(1, 1, nCols, nRows);
	for (size_t i = 2; i < nRows - 2 && vecPtr < entryVec.size(); ++i){
		if (i == vecPtr){
			std::cout << Terminal::Invert(true);
		}
		PrintEntryVertical(entryVec[i], i, menuGravity);
		if (i == vecPtr){
			std::cout << Terminal::Invert(false);
		}
	}
}

ssize_t Menu::MenuImpl::DisplayVertical(){
	size_t nRows = Terminal::GetRows();
	size_t nCols = Terminal::GetCols();
	size_t vecPtr = 0;
	int c;

	set_sigwinch_handler();
	if (setjmp(s_jmpbuf)){
		nRows = Terminal::GetRows();
	}

	do{
		PrintMenuVertical(nCols, nRows, vecPtr, entryVec, menuGravity);
		switch((c = Terminal::getch())){
		case KEY_DOWN:
		case 'S':
		case 'K':
			vecPtr++;
			break;
		case KEY_UP:
		case 'W':
		case 'J':
			vecPtr--;
			break;
		}
	}while (c != KEY_ENTER);

	remove_sigwinch_handler();
	return vecPtr;
}

ssize_t Menu::MenuImpl::DisplayHorizontal(){
	return 0;
}

Menu::Menu(){
	impl = std::make_unique<MenuImpl>();
}

Menu::Menu(std::vector<std::string>& entries){
	impl = std::make_unique<MenuImpl>();
	SetEntryVector(entries);
}

Menu& Menu::SetEntryVector(std::vector<std::string>& entries){
	impl->entryVec = entries;
	return *this;
}

Menu& Menu::SetMenuGravity(Alignment a){
	impl->menuGravity = a;
	return *this;
}

Menu& Menu::SetMenuAxis(Axis a){
	impl->axis = a;
	return *this;
}

ssize_t Menu::display(){
	if (impl->axis == VERTICAL){
		return impl->DisplayVertical();
	}
	else{
		return impl->DisplayHorizontal();
	}
}

}
