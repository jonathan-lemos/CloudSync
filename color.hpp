/** @file color.hpp
 * @brief ANSI color escape code constants.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_COLOR_HPP
#define __CS_COLOR_HPP

#include <ostream>

namespace CloudSync{
namespace Color{

#if defined(ENABLE_BRIGHT_COLORS)
const char* const COLOR_BLACK   = "\033[90m";
const char* const COLOR_RED     = "\033[91m";
const char* const COLOR_GREEN   = "\033[92m";
const char* const COLOR_YELLOW  = "\033[93m";
const char* const COLOR_BLUE    = "\033[94m";
const char* const COLOR_MAGENTA = "\033[95m";
const char* const COLOR_CYAN    = "\033[96m";
const char* const COLOR_WHITE   = "\033[97m";
const char* const COLOR_NORMAL  = "\033[m";
#elif defined (DISABLE_COLORS)
const char* const COLOR_BLACK   = "";
const char* const COLOR_RED     = "";
const char* const COLOR_GREEN   = "";
const char* const COLOR_YELLOW  = "";
const char* const COLOR_BLUE    = "";
const char* const COLOR_MAGENTA = "";
const char* const COLOR_CYAN    = "";
const char* const COLOR_WHITE   = "";
const char* const COLOR_NORMAL  = "";
#else
const char* const COLOR_BLACK   = "\033[30m";
const char* const COLOR_RED     = "\033[31m";
const char* const COLOR_GREEN   = "\033[32m";
const char* const COLOR_YELLOW  = "\033[33m";
const char* const COLOR_BLUE    = "\033[34m";
const char* const COLOR_MAGENTA = "\033[35m";
const char* const COLOR_CYAN    = "\033[36m";
const char* const COLOR_WHITE   = "\033[37m";
const char* const COLOR_NORMAL  = "\033[m";
#endif

}
}

#endif
