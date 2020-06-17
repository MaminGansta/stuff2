
#include "thread_pool.h"


// ========== Constructors and distructor ===========

ThreadPool::ThreadPool()
{
	int size = std::thread::hardware_concurrency();
	stopping = false;
	start(size);
}


ThreadPool::ThreadPool(size_t threads)
{
	int size = threads;
	stopping = false;
	start(size);
}


ThreadPool::~ThreadPool()
{
	stop();
}


// =========== Start and Stop ================

void ThreadPool::start(int size)
{
	resize(size);
}


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
						cv_wait.notify_one();
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


// ================ void add task =======================

std::future<void> ThreadPool::add_task_void(std::function<void()> task)
{
	// increment task counter
	active_tasks++;

	auto wrapper = std::make_shared<std::packaged_task<void()>>(std::move(task));
	{
		std::unique_lock<std::mutex> lock(event_mutex);
		tasks.push([=]() { (*wrapper)(); });
	}
	event.notify_one();
	return wrapper->get_future();
}


//  =============== void parallel for =========================

void ThreadPool::parallel_for_void(int from_param, int to_param, std::function<void(int, int)> func)
{
	// split whole task to pieces
	int width = to_param - from_param;
	int threads = __min(width, size());

	// put pisces on thread pool
	for (int i = 0; i < threads; i++)
	{
		int from = i * width / threads + from_param;
		int to = (i + 1) * width / threads + from_param;
		add_task(func, from, to);
	}
}