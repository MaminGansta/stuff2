
#include "thread_pool/thread_pool.h"
#include <stdio.h>

using namespace std::literals::chrono_literals;

int main(void)
{
	ThreadPool pool;

	// 1
	std::vector<int> a(1024, 1);
	
	auto res =
		pool.parallel_for(0, a.size(), [&a](int from, int to)->int
		{
			int sum = 0;
			for (int i = from; i < to; i++)
				sum += a[i];
	
			return sum;
		});
	
	int sum = 0;
	for (int i = 0; i < res.size(); i++)
		sum += res[i].get();
	
	printf("sum = %d\n\n", sum);
	
	
	// 2
	pool.add_task_void([]() {printf("void task\n");});
	
	std::future<int> future =
		pool.add_task([]() { printf("task: return value-> "); return 1; });
	
	printf("%d\n\n", future.get());
	
	
	// 3
	printf("thread pool size: %d\n", pool.size());
	
	pool.resize(16);
	printf("resize\n");
	
	printf("thread pool size: %d\n\n", pool.size());


	// 4 donw size
	printf("thread pool size: %d\n", pool.size());

	pool.resize(8);
	printf("down size\n");

	printf("thread pool size: %d\n\n", pool.size());


	// 5 
	pool.parallel_for_void(0, 5, [](int from, int to)
		{
			std::this_thread::sleep_for(1ms);
			printf("parallel for void\n");
		}
	);

	pool.wait();
	printf("wait for parallel_for_void\n");


	return 0;
}