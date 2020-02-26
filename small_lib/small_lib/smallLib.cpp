#include <cassert>
#include <memory.h>
#include <utility>
#include <malloc.h>

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
		array(const std::initializer_list<T>& args)
		{
			assert(args.size() <= capacity);
			int i = 0;
			size = args.size();
			for (size_t i = 0; i < size; i++)
				data[i] = *(args.begin() + i);
		}

		T& operator [] (int idx)
		{
			assert((unsigned)idx < size);
			return data[idx];
		}

		void push_back(T const && elem)
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
	struct smallvector
	{
		T* data;
		size_t size;

		smallvector(size_t size, T value) : size(size)
		{
			assert(sizeof(T) * size < 1024);
			data = (T*)_malloca(sizeof(T) * size);
			for (size_t i = 0; i < size; i++)
				new(data + i) T(value);
		}

		~smallvector()
		{
			for (size_t i = 0; i < size; i++)
				data[i].~T();
		}

	};


	template <typename T>
	struct vector
	{
		T* data = nullptr;
		size_t capacity = 0;
		size_t size = 0;

		vector() = default;
		vector(size_t capacity)
		{
			reserve(capacity);
			this->capacity = capacity;
		}

		vector(size_t size, const T& value)
		{
			reserve(size);
			capacity = size;
			for (size_t i = 0; i < size; i++)
				push_back(value);
		}

		vector(const std::initializer_list<T>& args)
		{
			reserve(args.size());
			size = capacity = args.size();
			for (int i = 0; i < size; i++)
				new(data + i) T(*(args.begin() + i));
		}

		~vector()
		{
			for (size_t i = 0; i < size; i++)
				data[i].~T();
			::operator delete(data);
		}

		void push_back(const T& value)
		{
			size_t new__size = size + 1;
			if (new__size > capacity) 
				grow(new__size);
			new(data + size) T(value);
			size = new__size;
		}

		void move_back(T& value)
		{
			size_t new__size = size + 1;
			if (new__size > capacity)
				grow(new__size);
			new(data + size) T(std::move(value));
			size = new__size;
		}

		T& operator [](int i)
		{
			assert((unsigned)i < size);
			return data[i];
		}

		void remove(int i)
		{
			assert((unsigned)i < size);
			using std::swap;
			swap(data[i], data[--size]);
			data[size].~T();
		}

		void erase(int i)
		{
			assert((unsigned)i < size);
			data[size].~T();
			memmove(data + i, data + i + 1, sizeof(T) * (size - i));
			size--;
		}

		void reserve(size_t reserve_size)
		{
			if (reserve_size <= capacity) return;
			
			T* new_buffer = (T*)::operator new(reserve_size * sizeof(T));
			
			assert(data == nullptr && size == 0);
			memmove(new_buffer, data, sizeof(T) * size);
			::operator delete(data);

			data = new_buffer;
			assert(data);
			capacity = reserve_size;
		}

		T* begin() { return data; }
		T* end() { return data + size; }

	private:

		void grow(size_t new_cap)
		{
			new_cap = MAX(2 * capacity + 1, new_cap);
			assert(new_cap > capacity);
			
			T* new_buffer = (T*)::operator new(new_cap * sizeof(T));
			assert(data != nullptr && size != 0);
			memmove(new_buffer, data, sizeof(T) * size);
			::operator delete(data);
			
			data = new_buffer;
			assert(data);
			capacity = new_cap;
		}

	};




}
