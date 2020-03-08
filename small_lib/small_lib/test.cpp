
#include "smallLib.cpp"

#include <iostream>
#include <utility>
#include <vector>


#include <unordered_map>



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
	mystruct& operator =(const mystruct& m)
	{
		p = new int(*m.p);
		std::cout << "copy oper\n";
		return *this;
	}
	mystruct(mystruct&& m)
	{
		p = m.p;
		m.p = nullptr;
		std::cout << "moved\n";
	}
	mystruct& operator= (mystruct&& m)
	{
		p = m.p;
		m.p = nullptr;
		a = m.a;
		std::cout << "moved\n";
		return *this;
	}

	~mystruct()
	{
		delete p;
	}
};


int main(void)
{
	
	using namespace small;
	// array
	array<mystruct, 100> a(10);
	auto b = a;
	auto c = std::move(a);

	c.remove(1);

	for (int i = 0; i < b.size; i++)
		a.push_back(b.front());


	queue<int, 10> aa;
	

	for (int i = 0; i < 10; i++)
		aa.push(i);

	for (int i = 0; i < 5; i++)
		std::cout << aa.get() << ' ';

	for (int i = 0; i < 5; i++)
		aa.push(i);

	//vector<std::pair<int, int>> container[2];
	//container[0].push_back(std::make_pair(1, 2));

	timer t;

	for (int i = 0; i < 100; i++)
		std::cout << frng(-100, 110) << '\n';

	std::cout << '\n' << t.elaped();
	return 0;
}