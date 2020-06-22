#pragma once

#include <thread>
#include <vector>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future>
#include <atomic>
#include <cassert>


struct ThreadPool
{
	std::vector<std::thread> pool;
	std::queue<std::function<void()>> tasks;

	std::condition_variable event;
	std::mutex event_mutex;
	std::atomic<bool> stopping;

	// task counter
	std::atomic<int> active_tasks = 0;

	std::mutex mutex_wait;
	std::condition_variable cv_wait;

	ThreadPool();
	ThreadPool(size_t threads);
	
	~ThreadPool();

	// add task
	template <typename F, typename ...Args>
	auto add_task(F&& task, Args&& ...args) ->std::future<decltype(task(args...))>
	{
		// increment active tasts
		active_tasks++;

		auto wrapper = std::make_shared<std::packaged_task<decltype(task(args...))()>>(
			std::bind(std::forward<F>(task), std::forward<Args>(args)...));

		{
			std::unique_lock<std::mutex> lock(event_mutex);
			tasks.push([wrapper]() { (*wrapper)(); });
		}
		event.notify_one();
		return wrapper->get_future();
	}


	/*
		Split range to small pieces
		Prototype: type F(int from, int to)
	*/
	template <typename F>
	auto parallel_for(int from_param, int to_param, F&& func) -> std::vector<std::future<decltype(func(1, 1))>>
	{
		std::vector<std::future<decltype(func(1, 1))>> res;
		res.reserve(pool.size());

		// split whole task to pieces
		int width = to_param - from_param;
		int threads = __min(width, size());

		// put pisces on thread pool
		for (int i = 0; i < threads; i++)
		{
			int from = i * width / threads + from_param;
			int to = (i + 1) * width / threads + from_param;
			res.push_back(std::move(add_task(func, from, to)));
		}
		return res;
	}


	/* 
		task return void (common use case)
		no code generation
	*/
	template <typename F>
	void add_task_void(F&& task)
	{
		// increment task counter
		active_tasks++;
		{
			std::unique_lock<std::mutex> lock(event_mutex);
			tasks.push([t = std::move(task)]() { t(); });
		}
		event.notify_one();
	}

	/*
		Less dynamic allocation, in this version
	*/
	template <typename F>
	void parallel_for_void(int from_param, int to_param, F&& func)
	{
		// split whole task to pieces
		int width = to_param - from_param;
		int threads = __min(width, size());

		// put pisces on thread pool
		for (int i = 0; i < threads; i++)
		{
			int from = i * width / threads + from_param;
			int to = (i + 1) * width / threads + from_param;
			
			active_tasks++;
			{
				std::unique_lock<std::mutex> lock(event_mutex);
				tasks.push([&func, from, to]() { func(from, to); });
			}
			event.notify_one();
		}
	}


	// wait until all tasks will be finished
	void wait();

	void resize(int size);

	int size();

	void stop() noexcept;
};