
#include "thread_pool/thread_pool.h"
#include <stdio.h>

using namespace std::literals::chrono_literals;


void* operator new(std::size_t sz) {
	std::printf("new called, size = %zu\n", sz);
	void* ptr = std::malloc(sz);
	if (ptr)
		return ptr;
	else
		throw std::bad_alloc{};
}
void operator delete(void* ptr) noexcept
{
	std::puts("delete called");
	std::free(ptr);
}


int main(void)
{
	printf("\nthread pool allocations\n\n");
	ThreadPool pool;

	// 1
	printf("\nvector allocations\n\n");
	std::vector<int> a(1024, 1);
	
	printf("\nparallel for allocations\n\n");
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
	
	//return 0;

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
	printf("\nvoid parallel for\n");
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