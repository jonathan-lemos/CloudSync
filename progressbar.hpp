/** @file progressbar.hpp
 * @brief Thread-safe progress bar class.
 * @copyright Copyright (c) 2018 Jonathan Lemos
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <cstdint>
#include <memory>

namespace CloudSync{

class ProgressBar{
public:
	ProgressBar(const char* msg = "", uint64_t max = 100, int intervalMillis = 500);
	~ProgressBar();
	void display();
	void incProgress(uint64_t amount);
	void setProgress(uint64_t amount);
	ProgressBar& setMsg(const char* msg);
	ProgressBar& setMax(uint64_t max);
	ProgressBar& setInterval(int intervalMillis);
	void finish();
	void fail();
	void reset();
	bool isActive();
private:
	struct ProgressBarImpl;
	std::unique_ptr<ProgressBarImpl> impl;
};

}
