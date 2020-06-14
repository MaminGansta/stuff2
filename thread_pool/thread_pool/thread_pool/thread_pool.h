#pragma once

#include <thread>
#include <vector>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future>
#include <atomic>

struct ThreadPool
{
	std::vector<std::thread> pool;
	std::queue<std::function<void()>> tasks;

	std::condition_variable event;
	std::mutex event_mutex;
	std::atomic<bool> stopping;


	ThreadPool();
	ThreadPool(size_t threads);
	
	~ThreadPool();

	// add task
	template <typename F, typename ...Args>
	auto add_task(F&& task, Args&& ...args) ->std::future<decltype(task(args...))>
	{
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
	auto parallel_for(int from_param, int to_param, F func) -> std::vector<std::future<decltype(func(1, 1))>>
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
			res.push_back(add_task(func, from, to));
		}

		return res;
	}


	/* 
		task return void (common use case)
		no code generation
	*/
	std::future<void> add_task_void(std::function<void()> task);

	/*
		No dynamic allocation, in this version
		no code generation
	*/
	void parallel_for_void(int from_param, int to_param, std::function<void(int, int)> func);

	// wait for parallel_for_void
	// tasks will be finished
	void wait();

	void resize(int size);

	int size();

private:
	// neccessary to avoid allocations in parallel_for
	std::vector<std::future<void>> result;

	void start(int size);

	void stop() noexcept;
};