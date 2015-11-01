#include <sstream>
#include <cassert>
#include "ThreadPool.hh"

namespace ahttpd 
{

ThreadPool::ThreadPool(size_t n, std::function<void()> on_enter, std::function<void()> on_exit)
	: workers(n)
{
	for(size_t i = 0; i < n; ++i)
		workers.emplace_back([=]{
			if(on_enter)
				on_enter();
			while(true)
			{
				std::unique_lock<std::mutex> lck(queue_mutex);

				condition.wait(lck, [this]{
					return stopped || !tasks.empty();
				});
				if(tasks.empty())
				{
					/** NOTE: Do nothing for spurious wakeup. */
					if(stopped)
						break;
				}
				else
				{
					const auto task(std::move(tasks.front()));

					tasks.pop();
					lck.unlock();
					try
					{
						task();
					}
					catch(std::future_error&)
					{
						assert(false);
					}
				}
			}
			if(on_exit)
				on_exit();
		});
}

ThreadPool::~ThreadPool() noexcept
{
	try
	{
		try
		{
			std::lock_guard<std::mutex> lck(queue_mutex);
			stopped = true;
		}
		catch(std::system_error& e)
		{
			assert(false);
		}
		condition.notify_all();
		for(auto& worker : workers)
			try
			{
				worker.join();
			}
			catch(std::system_error&)
			{}
	}
	catch(...)
	{
		assert(false);
	}
}

size_t ThreadPool::size() const
{
	std::unique_lock<std::mutex> lck(queue_mutex);

	return size_unlocked();
}

}	/**< namespace ahttpd */
