/** @file progressbar.cpp
 * @brief Thread-safe progress bar class.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "progressbar.hpp"
#include "io_mutex.hpp"
#include "terminal.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstring>

#include <sys/ioctl.h>
#include <unistd.h>

namespace CloudSync{
	const size_t metaWidth = strlen("[100.0%]");

	static size_t getTerminalWidth(){
		struct winsize size;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		return size.ws_col;
	}

	void ProgressBar::displayWait(int millis){
		std::unique_lock<std::mutex>lock(waitMutex);
		//wait until x milliseconds have passed or displayWorkerThreadActive is false
		cvWaitMutex.wait_for(lock, std::chrono::milliseconds(millis), [this]{return !displayWorkerThreadActive;});
	}

	void ProgressBar::displayWorker(){
		Terminal::ShowCursor(false);
		while (true){
			const size_t terminalWidth = getTerminalWidth();
			const size_t usableWidth = terminalWidth - metaWidth - 1;
			std::string displayMsg;
			double pct;
			std::stringstream pctFormat;

			//lock class data so another thread doesn't write to it while this thread is reading
			dataMutex.lock();
			//do not display over 100%
			if (curProgress > maxProgress){
				curProgress = maxProgress;
			}
			pct = (curProgress / maxProgress) * 100.0;
			displayMsg = msg;
			//do not need to read any more of the class's data
			dataMutex.unlock();

			if (displayMsg.length() > usableWidth){
				displayMsg.resize(usableWidth - 3);
				displayMsg.append("...");
			}
			displayMsg.append(usableWidth - displayMsg.length(), ' ');

			displayMsg.append("[");
			pctFormat << std::setprecision(2) << pct;
			displayMsg.append(pctFormat.str());
			displayMsg.append("]");

			IO::lock();
			std::cout << '\r' << displayMsg;
			IO::unlock();

			if (curProgress == maxProgress){
				displayWorkerThreadActive = false;
			}
			if (!displayWorkerThreadActive){
				break;
			}
			displayWait(interval);
		}
		Terminal::ShowCursor(true);
	}

	ProgressBar::ProgressBar(const char* msg, uint64_t max, int interval): interval(interval), msg(msg), maxProgress(max){}

	ProgressBar::~ProgressBar(){
		if (isActive()){
			fail();
		}
	}

	void ProgressBar::display(){
		if (isActive()){
			return;
		}
		displayWorkerThread = std::thread(&ProgressBar::displayWorker, this);
		displayWorkerThreadActive = true;
	}

	void ProgressBar::incProgress(uint64_t amount){
		dataMutex.lock();
		curProgress += amount;
		dataMutex.unlock();
	}

	void ProgressBar::setProgress(uint64_t amount){
		dataMutex.lock();
		curProgress = amount;
		dataMutex.unlock();
	}

	ProgressBar& ProgressBar::setMsg(const char* msg){
		dataMutex.lock();
		this->msg = msg;
		dataMutex.unlock();
		return *this;
	}

	ProgressBar& ProgressBar::setMax(uint64_t max){
		dataMutex.lock();
		maxProgress = max;
		dataMutex.unlock();
		return *this;
	}

	ProgressBar& ProgressBar::setInterval(int intervalMillis){
		dataMutex.lock();
		interval = intervalMillis;
		dataMutex.unlock();
		return *this;
	}

	void ProgressBar::finish(){
		if (!isActive()){
			return;
		}

		displayWorkerThreadActive = false;
		cvWaitMutex.notify_all();

		const size_t terminalWidth = getTerminalWidth();
		const size_t usableWidth = terminalWidth - metaWidth - 1;
		std::string displayMsg;

		dataMutex.lock();
		displayMsg = msg;
		dataMutex.unlock();

		if (displayMsg.length() > usableWidth){
			displayMsg.resize(usableWidth - 3);
			displayMsg.append("...");
		}
		displayMsg.append(usableWidth - displayMsg.length(), ' ');
		displayMsg.append("[100.0%]");

		IO::lock();
		std::cout << '\r' << displayMsg << std::endl;
		IO::unlock();
	}

	void ProgressBar::fail(){
		if (!isActive()){
			return;
		}

		displayWorkerThreadActive = false;
		cvWaitMutex.notify_all();

		IO::lock();
		std::cout << std::endl;
		IO::unlock();
	}

	void ProgressBar::reset(){
		if (isActive()){
			fail();
		}
		curProgress = 0;
	}

	bool ProgressBar::isActive(){
		return displayWorkerThreadActive;
	}
}
