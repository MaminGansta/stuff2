
#include "smallLib.cpp"

#include <iostream>
#include <utility>
#include <vector>

struct mystruct
{
	int a;
	int* p;

	mystruct() { p = new int(5); a = 1; std::cout << "cosntr\n"; }
	
	mystruct(const mystruct& m)
	{
		a = m.a;
		p = new int(3);
		std::cout << "copy\n";
	}

	mystruct& operator =(mystruct& m)
	{
		std::cout << "copy oper\n";
		return *this;
	}
	
	mystruct(mystruct&& m)
	{
		p = m.p;
		m.p = nullptr;
		std::cout << "moved\n";
	}

	~mystruct()
	{
		delete p;
	}
};

int main(void)
{
	

	using namespace small;

	smallvector<mystruct> a(3, mystruct());

	//vector<mystruct> b(3, mystruct());

	return 0;
}