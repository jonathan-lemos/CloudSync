/** @file menu.hpp
 * @brief Displays a menu to the user.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __MENU_HPP
#define __MENU_HPP

#include <vector>
#include <string>
#include <memory>

namespace CloudSync{

class Menu{
public:
	enum Alignment{
		LEFT,
		RIGHT,
		CENTER
	};

	enum Axis{
		VERTICAL,
		HORIZONTAL
	};

	Menu();
	Menu(std::vector<std::string>& entries);

	Menu& SetEntryVector(std::vector<std::string>& entries);
	Menu& SetMenuGravity(Alignment a);
	Menu& SetMenuAxis(Axis a);

	ssize_t display();
private:
	struct MenuImpl;
	std::unique_ptr<MenuImpl> impl;
};

}

#endif
