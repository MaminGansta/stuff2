
#include "thread_pool.h"


// ========== Constructors and distructor ===========

ThreadPool::ThreadPool()
{
	int size = std::thread::hardware_concurrency();
	stopping = false;
	resize(size);
}

ThreadPool::ThreadPool(size_t threads)
{
	int size = threads;
	stopping = false;
	resize(size);
}

ThreadPool::~ThreadPool()
{
	stop();
}


// =========== Stop ================

void ThreadPool::stop() noexcept
{
	stopping = true;
	event.notify_all();

	for (std::thread& thread : pool)
		thread.join();
}


// ================ resize =================

void ThreadPool::resize(int new_size)
{
	// free all threads if down size
	if (new_size < size())
	{
		stopping = true;
		event.notify_all();
		for (std::thread& thread : pool)
			thread.join();
		pool.clear();
	}

	int new_threads = new_size - size();
	stopping = false;

	// create new threads
	for (int i = 0; i < new_threads; i++)
	{
		pool.push_back(std::thread([&]()
			{
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(event_mutex);

						event.wait(lock, [&]() { return stopping || !tasks.empty(); });
						if (stopping && tasks.empty()) break;

						task = std::move(tasks.front());
						tasks.pop();
					}
					task();

					// dencrement active tasts
					active_tasks--;

					// something going wrong
					assert(active_tasks >= 0);

					if (active_tasks == 0)
						cv_wait.notify_all();
				}
			})
		);
	}

}


//  ================= size =======================

int ThreadPool::size()
{
	return pool.size();
}


// =================== wait =======================

void ThreadPool::wait()
{
	std::unique_lock<std::mutex> lock(mutex_wait);
	cv_wait.wait(lock, [&]() { return active_tasks == 0; });
}