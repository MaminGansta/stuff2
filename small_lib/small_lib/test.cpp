
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

	hashmap<int, int, 2> map;
	map[1] = 2;


	hashmap<std::string, int, 2> mapp;

	mapp["dsf"] = 3;
	mapp["dsf"] = 3;
	mapp["fdsf"] = 21;
	mapp["dsdfsf"] = 1;
	mapp["dsasdff"] = 31;

	mapp.add("dsf", 3);

	mapp.has("dsf");
	mapp.remove("dsf");
	mapp.has("dsf");



	//vector<std::pair<int, int>> container[2];
	//container[0].push_back(std::make_pair(1, 2));


	return 0;
}