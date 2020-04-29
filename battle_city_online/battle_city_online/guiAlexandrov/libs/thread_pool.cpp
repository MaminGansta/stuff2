#include <cmath>
#include <thread>
#include <vector>
#include <algorithm>
#include <condition_variable>
#include <functional>
#include <queue>
#include <future>

#define MIN(a, b) (a < b ? a : b)

struct thread_pool
{
	size_t size;
	std::vector<std::thread> pool;
	std::queue<std::function<void()>> tasks;
	std::condition_variable event;
	std::mutex event_mutex;
	bool stopping;



	thread_pool(size_t threads = 8)
	{
		size = MIN(std::thread::hardware_concurrency(), threads);
		stopping = false;
		start();
	}

	~thread_pool() { stop(); }

	template <typename T>
	auto add_task(T task)->std::future<decltype(task())>
	{
		auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
		{
			std::unique_lock<std::mutex> lock(event_mutex);
			tasks.push([=]() { (*wrapper)(); });
		}
		event.notify_one();
		return wrapper->get_future();
	}

private:

	void start()
	{
		for (int i = 0; i < size; i++)
		{
			pool.push_back(std::thread([&]() {
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
				}
			}));
		}
	}

	void stop() noexcept
	{
		{
			std::unique_lock<std::mutex> lock(event_mutex);
			stopping = true;
		}

		event.notify_all();

		for (std::thread& thread : pool)
			thread.join();
	}
};



// lamda with necessary params [from, to](){ for (int i = from; i < to; i++}
#define ASYNC_FOR(from_param, to_param)											\
			{																	\
				std::future<void> res[MAX_THREADS];								\
				int af_width = to_param - from_param;							\
				for (int i = 0; i < workers.size; i++)							\
				{																\
					int from = i * af_width / workers.size + from_param;		\
					int to = (i + 1) * af_width / workers.size + from_param;	\
					res[i] = workers.add_task(



#define END_FOR																	\
					);															\
				}																\
																				\
				for (int i = 0; i < workers.size; i++)							\
					res[i].get();												\
			}