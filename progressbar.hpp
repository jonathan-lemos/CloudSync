/** @file progressbar.hpp
 * @brief Thread-safe progress bar class.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <cstdint>
#include <ctime>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace CloudSync{

	class ProgressBar{
	public:
		ProgressBar(const char* msg, uint64_t max = 100, int intervalMillis = 500);
		~ProgressBar();
		void display();
		void incProgress(uint64_t amount);
		void setProgress(uint64_t amount);
		ProgressBar& setMsg(const char* msg);
		ProgressBar& setMax(uint64_t max);
		ProgressBar& setInterval(int intervalMillis);
		void finish();
		void fail();
	private:
		void displayWorker();
		void displayWait(int millis);

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

}
