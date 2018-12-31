/** @file progressbar.hpp
 * @brief Displays a progress message to the screen.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "progressbar.hpp"
#include "terminal.hpp"
#include <chrono>
#include <cstring>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>

namespace CloudSync {

constexpr const char spinnerStages[] = { '|', '/', '-', '\\' };

struct ProgressBar::ProgressBarImpl {
	/**
	 * @brief True if the ProgressBar is active, false if not.
	 */
	bool active = false;

	/**
	 * @brief The message the ProgressBar displays.
	 */
	const char* msg = "";

	/**
	 * @brief The current progress of this ProgressBar.
	 */
	uint64_t curProgress = 0;

	/**
	 * @brief The max progress of this ProgressBar.
	 */
	uint64_t maxProgress = 100;

	/**
	 * @brief How often to update the ProgressBar in milliseconds.
	 */
	int intervalMillis = 100;

	/**
	 * @brief What stage the spinner is on.
	 */
	int spinnerStage = 0;

	/**
	 * @brief The worker thread that displays the bar.
	 */
	std::thread workerThread;

	/**
	 * @brief The condition variable that wakes up the thread if it's no longer active.
	 */
	std::condition_variable cv;

	/**
	 * @brief Needed to create a lock for the condition variable.
	 */
	std::mutex mCv;

	/**
	 * @brief Needed to prevent data races.
	 */
	std::mutex mData;

	/**
	 * @brief This function displays the progress on a timer.
	 */
	void progressWorker() {
		// This is needed for the condition variable to work.
		std::unique_lock<std::mutex> lockCv(this->mCv);

		// do-while so this executes at least once even when this->active is false
		do {
			// Lock the data to prevent data races.
			std::unique_lock<std::mutex> lockData(this->mData);
			// Get the length of the ending percentage expression.
			constexpr size_t exprLen = sizeof("[000.00%]|") - 1;
			// Width is the amount of remaining columns - 2 to prevent overflowing to the next line.
			const size_t width = Terminal::GetCols() - 2 - exprLen;
			// Get the current progress as a percentage.
			const double progPct = (double)this->curProgress / (double)this->maxProgress;
			// Get the amount of the bar that should be filled in characters.
			const size_t progLen = (progPct * (width - exprLen));

			// Return to the beginning of the line and invert the text.
			std::cout << '\r' << Terminal::Invert(true);
			for (size_t i = 0; i < width; ++i) {
				// If we reach the point we should stop inverting.
				if (i == progLen) {
					std::cout << Terminal::Invert(false);
				}
				// Print chars until we can't anymore
				if (i < strlen(this->msg)) {
					std::cout << this->msg[i];
				}
				else {
					std::cout << " ";
				}
			}
			// Stop inverting if we haven't done so already
			std::cout << Terminal::Invert(false);
			// Print the percentage expression
			std::cout << "[" << std::setw(6) << std::setprecision(2) << std::right << (progPct * 100) << "]" << spinnerStages[this->spinnerStage];
			// Send the spinner to the next stage
			this->spinnerStage++;
			this->spinnerStage %= sizeof(spinnerStages) / sizeof(spinnerStages[0]);

			// We no longer need a lock on the data, so release it
			lockData.release();
			// Wait until intervalMillis pass or this->active is false and this cv is notified by another thread.
			this->cv.wait_for(lockCv, std::chrono::milliseconds(this->intervalMillis), [this]{ return this->active; } );
		// Until this->active is set to false by another thread.
		} while (this->active);
	}
};

ProgressBar::ProgressBar(const char* msg, uint64_t max, int intervalMillis): impl(std::make_unique<ProgressBarImpl>()) {
	this->impl->msg = msg;
	this->impl->maxProgress = max;
	this->impl->intervalMillis = intervalMillis;
}

ProgressBar::~ProgressBar() {
	if (this->impl->active) {
		this->fail();
	}
}

void ProgressBar::display() {
	this->impl->active = true;
	this->impl->workerThread = std::thread(&ProgressBarImpl::progressWorker, this->impl.get());
}

ProgressBar& ProgressBar::incProgress(uint64_t amount) {
	std::unique_lock<std::mutex> lock(this->impl->mData);
	this->impl->curProgress += amount;
	return *this;
}

ProgressBar& ProgressBar::setProgress(uint64_t amount) {
	std::unique_lock<std::mutex> lock(this->impl->mData);
	this->impl->curProgress = amount;
	return *this;
}

ProgressBar& ProgressBar::setMsg(const char* msg) {
	std::unique_lock<std::mutex> lock(this->impl->mData);
	this->impl->msg = msg;
	return *this;
}

ProgressBar& ProgressBar::setMax(uint64_t max) {
	std::unique_lock<std::mutex> lock(this->impl->mData);
	this->impl->maxProgress = max;
	return *this;
}

ProgressBar& ProgressBar::setInterval(int intervalMillis) {
	std::unique_lock<std::mutex> lock(this->impl->mData);
	this->impl->intervalMillis = intervalMillis;
	return *this;
}

void ProgressBar::finish() {
	this->impl->active = false;
	this->impl->cv.notify_all();
	this->impl->curProgress = this->impl->maxProgress;
	this->impl->progressWorker();
}

void ProgressBar::fail() {
	this->impl->active = false;
	this->impl->cv.notify_all();
}

void ProgressBar::reset() {
	this->impl->active = false;
	this->impl->cv.notify_all();
	this->impl->curProgress = 0;
}

bool ProgressBar::isActive() {
	return this->impl->active;
}

}
