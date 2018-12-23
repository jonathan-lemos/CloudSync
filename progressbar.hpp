/** @file progressbar.hpp
 * @brief Displays a progress message to the screen.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef __CS_PROGRESSBAR_HPP
#define __CS_PROGRESSBAR_HPP

#include <cstdint>
#include <memory>

namespace CloudSync{

class ProgressBar{
public:
	/**
	 * @brief Constructs a ProgressBar
	 *
	 * @param msg The message to display in the progress bar.
	 * @param max The maximum value of the progress bar.
	 * @param intervalMillis How quickly the progress bar should be updated.
	 */
	ProgressBar(const char* msg = "", uint64_t max = 100, int intervalMillis = 100);
	/**
	 * @brief Stops a ProgressBar if it is currently active.
	 */
	~ProgressBar();

	/**
	 * @brief Displays a ProgressBar
	 * If one is currently active, this waits for the other to exit.
	 */
	void display();

	/**
	 * @brief Increments the progress in the ProgressBar
	 *
	 * @param amount The amount to increment it by.
	 *
	 * @return this
	 */
	ProgressBar& incProgress(uint64_t amount);

	/**
	 * @brief Sets the progress in the ProgressBar
	 *
	 * @param amount The amount to set it to.
	 *
	 * @return this
	 */
	ProgressBar& setProgress(uint64_t amount);

	/**
	 * @brief Sets the message displayed in the progress bar.
	 *
	 * @param msg The message to set.
	 *
	 * @return this
	 */
	ProgressBar& setMsg(const char* msg);

	/**
	 * @brief Sets the maximum value of the progress bar.
	 *
	 * @param max The maximum value to set.
	 *
	 * @return this.
	 */
	ProgressBar& setMax(uint64_t max);

	/**
	 * @brief Sets the update interval of the progress bar.
	 *
	 * @param intervalMillis How often to update the progress bar in milliseconds.
	 */
	ProgressBar& setInterval(int intervalMillis);

	/**
	 * @brief Sets the progress to 100% and exits the progress bar.
	 */
	void finish();

	/**
	 * @brief Leaves the progress as-is and exits the progress bar.
	 */
	void fail();

	/**
	 * Returns if the progress bar is currently displaying.
	 *
	 * @return True if the progress bar is displaying, false if not.
	 */
	bool isActive();

private:
	struct ProgressBarImpl;
	std::unique_ptr<ProgressBarImpl> impl;
};

}

#endif
