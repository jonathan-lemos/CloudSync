/** @file attribute.h
 * @brief gcc optimization attributes.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __ATTRIBUTE_H
#define __ATTRIBUTE_H

#ifndef __GNUC__
#define __attribute__(x)
#endif

/**
 * A function marked with this attribute is unlikely to be executed, so it is optimized for space instead of speed.
 */
#define CS_COLD __attribute__((cold))

/**
 * A function or variable marked with this attribute is deprecated and a warning is given if it is used.
 *
 * @param reason The reason why this function or variable is deprecated.
 */
#define CS_DEPRECATED(reason) __attribute__((deprecated(reason)))

/**
 * A function marked with this attribute is executed often, so it is optimized for speed instead of space.
 */
#define CS_HOT __attribute__((hot))

/**
 * A function marked with this attribute is always inlined.
 */
#define CS_INLINE __attribute__((always_inline))

/**
 * A function marked with this attribute is malloc-like, meaning its return pointer is unique, and its return value contains no existing pointers.
 */
#define CS_MALLOC_LIKE __attribute__((malloc))

/**
 * A function marked with this attribute is treated as a printf-like function, and appropriate compiler warnings will be generated for improper usage.<br>
 * For example, the following will all generate warnings:
 * ```C
 * printf("%d", (char*)string);                     // Passing char* to argument of type int.
 * printf("%d %d", (int)num);                       // 2 arguments needed but 1 given.
 * printf("%d %d", (int)num1, (int)num2, (int)num3) // 2 arguments needed but 3 given.
 * ```
 *
 * @param index The index of the printf format string.
 */
#define CS_PRINTF_LIKE(index) __attribute__((format(printf,index,index + 1)))

/**
 * A function marked with this attribute will always produce the same output given the same input.<br>
 * It also has no side effects and does not rely on global variables.
 */
#define CS_PURE __attribute__((const))

/**
 * A function marked with this attribute is potentially unused.<br>
 * An "unused function" warning will not be generated for this function.
 */
#define CS_UNUSED __attribute__((unused))

#endif
