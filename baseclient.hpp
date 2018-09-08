/** @file baseclient.hpp
 * @brief Base client class that needs to be implemented.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_BASECLIENT_HPP
#define __CS_BASECLIENT_HPP

#include <sys/stat.h>
#include <vector>
#include <string>
#include <optional>

namespace CloudSync{

/**
 * Cloud client abstract superclass.<br>
 * This is used to connect to a cloud service.<br>
 * Do not instantiate the BaseClient directly; use one of its subclasses.
 */
class BaseClient{
	public:
		/**
		 * The destructor must be virtual to prevent memory leaks when deleting a subclass from a base pointer. Thanks C++...
		 */
		virtual ~BaseClient() = 0;

		/**
		 * Logs into the cloud service.
		 *
		 * @param username The username/email to log into.
		 *
		 * @param password The password to log in with.
		 *
		 * @return True if the login was successful, false if not.
		 */
		virtual bool login(const char* username, const char* password) = 0;

		/**
		 * Creates a directory.
		 *
		 * @param dir The directory to create.
		 *
		 * @return True if the directory was successfully created, false if not.
		 */
		virtual bool mkdir(const char* dir) = 0;

		/**
		 * Lists all the filenames in a directory.
		 *
		 * @param dir The directory to list.
		 *
		 * @return A list of filenames in the directory, or std::nullopt on error.
		 */
		virtual std::optional<std::vector<std::string>> readdir(const char* dir) = 0;

		/**
		 * Stats a directory/file in a cloud directory.
		 *
		 * @param dir The directory/file to stat.
		 *
		 * @param st A pointer to a stat structure that this function will fill.
		 *
		 * @return True if stat was successful, false if not.
		 */
		virtual bool stat(const char* path, struct stat* st) = 0;

		/**
		 * Moves a file in a cloud directory.
		 *
		 * @param old_path The old path of the file.
		 *
		 * @param new_path The new path of the file.
		 *
		 * @return True if moving was successful, false if not.
		 */
		virtual bool move(const char* old_path, const char* new_path) = 0;

		/**
		 * Downloads a file from the cloud.
		 *
		 * @param cloud_path The file to be downloaded.
		 *
		 * @param disk_path The location the file should be downloaded to.
		 *
		 * @return True if the download was successful, false if not.
		 */
		virtual bool download(const char* cloud_path, const char* disk_path) = 0;

		/**
		 * Uploads a file to the cloud.
		 *
		 * @param disk_path The file to be uploaded.
		 *
		 * @param cloud_path The location the file should be uploaded to.
		 *
		 * @return True if the upload was sucessful, false if not.
		 */
		virtual bool upload(const char* disk_path, const char* cloud_path) = 0;

		/**
		 * Removes a file or empty directory from the cloud.
		 *
		 * @param path The path of the file or directory that should be removed.
		 *
		 * @return True if the file or directory was sucessfully removed, false if not.
		 */
		virtual bool remove(const char* path) = 0;

		/**
		 * Logs out of the cloud service.
		 *
		 * @return True if the logout was successful, false if not.
		 */
		virtual bool logout() = 0;
};

}

#endif
