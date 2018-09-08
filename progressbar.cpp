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
#include <thread>
#include <mutex>
#include <condition_variable>

#include <sys/ioctl.h>
#include <unistd.h>

namespace CloudSync{

const size_t metaWidth = strlen("[100.0%]");

static size_t getTerminalWidth(){
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	return size.ws_col;
}

struct ProgressBarImpl{
	ProgressBarImpl(const char* msg, uint64_t max, int intervalMillis): interval(intervalMillis), msg(msg), maxProgress(max){}

	void displayWorker(){
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

			IO::io_mutex.lock();
			std::cout << '\r' << displayMsg;
			IO::io_mutex.unlock();

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

	void displayWait(int millis){
		std::unique_lock<std::mutex>lock(waitMutex);
		//wait until x milliseconds have passed or displayWorkerThreadActive is false
		cvWaitMutex.wait_for(lock, std::chrono::milliseconds(millis), [this]{return !displayWorkerThreadActive;});
	}

	std::thread displayWorkerThread;
	bool displayWorkerThreadActive = false;
	std::mutex dataMutex;
	std::mutex waitMutex;
	std::condition_variable cvWaitMutex;
	int interval;
	const char* msg = nullptr;
	uint64_t curProgress;
	uint64_t maxProgress;
};

ProgressBar::ProgressBar(const char* msg, uint64_t max, int interval): impl(std::make_unique<ProgressBarImpl>(msg, max, interval)){}

ProgressBar::~ProgressBar(){
	if (isActive()){
		fail();
	}
}

void ProgressBar::display(){
	if (isActive()){
		return;
	}
	impl->displayWorkerThread = std::thread(&ProgressBarImpl::displayWorker, this->impl.get());
	impl->displayWorkerThreadActive = true;
}

void ProgressBar::incProgress(uint64_t amount){
	impl->dataMutex.lock();
	impl->curProgress += amount;
	impl->dataMutex.unlock();
}

void ProgressBar::setProgress(uint64_t amount){
	impl->dataMutex.lock();
	impl->curProgress = amount;
	impl->dataMutex.unlock();
}

ProgressBar& ProgressBar::setMsg(const char* msg){
	impl->dataMutex.lock();
	impl->msg = msg;
	impl->dataMutex.unlock();
	return *this;
}

ProgressBar& ProgressBar::setMax(uint64_t max){
	impl->dataMutex.lock();
	impl->maxProgress = max;
	impl->dataMutex.unlock();
	return *this;
}

ProgressBar& ProgressBar::setInterval(int intervalMillis){
	impl->dataMutex.lock();
	impl->interval = intervalMillis;
	impl->dataMutex.unlock();
	return *this;
}

void ProgressBar::finish(){
	if (!isActive()){
		return;
	}

	impl->displayWorkerThreadActive = false;
	impl->cvWaitMutex.notify_all();

	const size_t terminalWidth = getTerminalWidth();
	const size_t usableWidth = terminalWidth - metaWidth - 1;
	std::string displayMsg;

	impl->dataMutex.lock();
	displayMsg = impl->msg;
	impl->dataMutex.unlock();

	if (displayMsg.length() > usableWidth){
		displayMsg.resize(usableWidth - 3);
		displayMsg.append("...");
	}
	displayMsg.append(usableWidth - displayMsg.length(), ' ');
	displayMsg.append("[100.0%]");

	IO::io_mutex.lock();
	std::cout << '\r' << displayMsg << std::endl;
	IO::io_mutex.unlock();
}

void ProgressBar::fail(){
	if (!isActive()){
		return;
	}

	impl->displayWorkerThreadActive = false;
	impl->cvWaitMutex.notify_all();

	IO::io_mutex.lock();
	std::cout << std::endl;
	IO::io_mutex.unlock();
}

void ProgressBar::reset(){
	if (isActive()){
		fail();
	}
	impl->curProgress = 0;
}

bool ProgressBar::isActive(){
	return impl->displayWorkerThreadActive;
}

}
