
namespace small

{
	template <typename T, size_t capacity>
	struct array
	{
		T data;
		size_t size;

		array() : size(0) { data = new T[capacity]; }

	};


}
