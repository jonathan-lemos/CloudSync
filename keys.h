/** @file cloud/keys.h
 *
 * Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __KEYS_H
#define __KEYS_H

/*
 * You must create a file called "keys.c" that defines the following constants.
 */

/**
 * @brief The MEGA API key to use.<br>
 * You must generate your own MEGA API key for use with this program.<br>
 * You can create one <a href=https://mega.nz/sdk>here</a>.<br>
 * <br>
 * Once this is done, create a file called "keys.c" that defines this constant like so:<br>
 * ```C
 * const char* const MEGA_API_KEY = "ABCDEFGH";
 * ```
 */
extern const char* const MEGA_API_KEY;

/**
 * @brief The MEGA sample username to use for testing purposes.<br>
 * You must generate your own MEGA account for use with this program.<br>
 * You can create one <a href=https://mega.nz/register>here</a>.<br>
 * <br>
 * Once this is done, create a file called "keys.c" that defines this constant like so:<br>
 * ```C
 * const char* const MEGA_SAMPLE_USERNAME = "johndoe@example.com";
 * ```
 */
extern const char* const MEGA_SAMPLE_USERNAME;

/**
 * @brief The MEGA sample password to use for testing purposes.<br>
 * You must generate your own MEGA account for use with this program.<br>
 * You can create one <a href=https://mega.nz/register>here</a>.<br>
 *
 * Once this is done, create a file called "keys.c" that defines this constant like so:<br>
 * ```C
 * const char* const MEGA_SAMPLE_PASSWORD = "hunter2";
 * ```
 */
extern const char* const MEGA_SAMPLE_PASSWORD;

#endif
