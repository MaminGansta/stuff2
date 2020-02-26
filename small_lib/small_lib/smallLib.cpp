#include <cstdlib>
#include <cassert>
#include <memory.h>

#ifndef MAX
#define MAX(a,b) (a > b? a : b)
#endif

namespace small
{

	template <typename T, size_t capacity>
	struct array
	{
		T data[capacity];
		size_t size;

		array() : size(0) {}

		T& operator [] (int idx)
		{
			assert((unsigned)idx < size);
			return data[idx];
		}

		void push_back(const T&& elem)
		{
			assert(size <= capacity);
			data[size++] = elem;
		}

		T& pop()
		{
			return data[--size];
		}
	};

	template <typename T>
	struct vector
	{
		T* data;
		size_t capacity;
		size_t size;

		vector() : data(nullptr), size(0), capacity(0) {}
		vector(int capacity) : data(nullptr), size(0), capacity(capacity)
		{

		}

		vector(size_t size, T const& value = T{}) : data(nullptr), capacity(size)
		{
			grow(capacity);
			for (int i = 0; i < size; i++)
				push_back(value);
		}

		~vector()
		{
			for (int i = 0; i < size; i++)
				data[i].~T();
			::operator delete(data);
		}

		void push_back(const T && value)
		{
			size_t new__size = size + 1;
			if (new__size > capacity) 
				grow(new__size);
			new(data + size) T(value);
			size = new__size;
		}

		T& operator [](int i)
		{
			assert((unsigned)i < size);
			return data[i];
		}

		void erise(int i)
		{
			assert((unsigned)i < size);
			data[i].~T();
			memmove(data + i, data + i + 1, sizeof(T) * (size - i));
		}

		void reserve(size_t reserve_size)
		{
			reserve_mem(reserve_size);
		}

		T* begin() { return data; }
		T* end() { return data + size; }

	private:

		void grow(size_t new_cap)
		{
			new_cap = MAX(2 * capacity + 1, new_cap);
			assert(new_cap > capacity);
			T* new_buffer = static_cast<T*>(::operator new(new_cap * sizeof(T)));

			// this shit must be tested
			//if (data)
			//{
				assert(data == nullptr && size != 0);
				memmove(new_buffer, data, sizeof(T) * size);
				::operator delete(data);
			//}
			data = new_buffer;
			assert(data);
			capacity = new_cap;
		}

		void reserve_mem(size_t new_cap)
		{

		}
	};




}
