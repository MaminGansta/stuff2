
/*
	try to solve  a + 2b + 5c = 15  using breadetic algorithm where a,b c unknown
*/

#include <ctime>
#include <array>
#include <random>
#include <cstdlib>
#include <iostream>
#include <functional>

#include "smallLib.cpp"

std::random_device rd;
std::mt19937 gen(rd());

// gray code lookup
unsigned char togray[16] = { 0b0000, 0b0001, 0b0011 ,0b0010,0b0110 ,0b0111 ,0b0101,
			0b0100 ,0b1100 ,0b1101 ,0b1111 ,0b1110 ,0b1010 ,0b1011 ,0b1001 ,0b1000 };

unsigned char from_gray[16]{ 0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11, 9, 8 };


struct mybread
{
	union
	{
		unsigned int whole;
		char chromo[4];
	};

	bool operator !=(mybread other)
	{
		return (whole != other.whole);
	}

	static mybread crossover(std::pair<mybread, mybread> pair)
	{
		std::uniform_int_distribution<> dis(0, sizeof(char) * 3 * 8);
		int breakpoint = dis(gen) + 8;
		int right = (sizeof(char) * 4 * 8) - breakpoint;

		mybread child = { (pair.second.whole << breakpoint >> breakpoint) | (pair.second.whole >> right << right) };
		// mutaion
		for (int i = 0; i < 32; i++)
			if (rand() % 100 < 20)
				child.whole = child.whole ^ 1 << i;

		return child;
	}

	static float fitnes(mybread unit)
	{
		float temp = 1.0f / (abs(15 - (unit.chromo[0] + 2*unit.chromo[1] + 5*unit.chromo[2])) + 1);
		return temp;
	}

	static bool check_res(mybread test)
	{
		return (test.chromo[0] + 2 * test.chromo[1] + 5 * test.chromo[2] == 15);
	}
};


// create new generaion
int cycles = 0;

template <typename bread, size_t size>
small::array<bread, size> evolution(small::array<bread, size> priv_generation)
{
	// calculate fitnes
	float fitnes_value[size];
	float fitnes_total = 0;
	float fitnes_avg = 0;

	for (int i = 0; i < size; i++)
	{
		fitnes_value[i] = bread::fitnes(priv_generation[i]);
		fitnes_total += fitnes_value[i];
	}

	fitnes_avg = fitnes_total / size;
	float fitnes_coef[size];
	// coef of parent validity
	for (int i = 0; i < size; i++)
		fitnes_coef[i] = fitnes_value[i] / fitnes_avg;

	// choose parents for new generation
	small::array<bread, size> parents;
	for (int i = 0; i < size; i++)
	{
		while (fitnes_coef[i] > 1.0f && parents.size < size)
		{
			fitnes_coef[i]--;
			parents.push_back(priv_generation[i]);
		}
	}

	// random loop
	while (parents.size < size)
	{
		std::uniform_real_distribution<> dis(0.0f, fitnes_total);
		for (int i = 0; i < size; i++)
		{
			cycles++;
			if (parents.size == size) break;
			if (fitnes_value[i] > dis(gen))
				parents.push_back(priv_generation[i]);
		}
	}

	small::array<bread, size> children;

	// find closest idntity from privius generation
	int index = 0; float max = fitnes_value[0];
	for (int i = 1; i < size; i++)
		if (max < fitnes_value[i])
			max = fitnes_value[i], index = i;
	
	// best parent move to the next generation automaticly
	children.push_back(priv_generation[index]);

	// make pairs ramdomly to cross them and get children
	for (int i = 1; i < size; i++)
	{
		int first = rand() % 5;
		int second = rand() % 5;
		if (first == second) second = (second + 3) % 5;
		children.push_back(bread::crossover(std::make_pair(parents[first], parents[second])));
	}

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			std::cout << (int)priv_generation[i].chromo[j] << ' ';
		}
		std::cout << "  " << fitnes_value[i] << '\n';
	}

	return children;
}



// core

template <typename bread, size_t size>
small::array<bread, size> gen_alg(small::array<bread, size> start)
{
	int time = 0;
	small::array<bread, size> step = start;
	bool ans = false;

	while (time < 1000 && !ans)
	{
		std::cout << "step " << time << "================" << '\n';
		
		// make new population
		step = evolution<bread, size>(step);
		time++;

		// check if we have solution
		for (int i = 0; i < size; i++)
		{
			if (bread::check_res(step[i]))
			{
				std::cout << "\nnumber: " << i << "\nresult: ";
				for (int j = 0; j < 3; j++)
					std::cout << (int)step[i].chromo[j] << ' ';
				std::cout << "\n";

				// just to see last genration
				evolution<bread, size>(step);
				ans = true;
			}
		}
	}

	return step;
}

int main(void)
{
	srand(time(0));
	std::uniform_int_distribution<unsigned int> roll(1 << 20, 1 << 31);

	small::array<mybread, 5> start;

	for (int i = 0; i < start.cap; i++)
	{
		unsigned int temp = rand();
		start.push_back(mybread{ roll(gen) });
	}

	auto res = gen_alg(start);

	std::cout  <<  "\nwasted cycles in a random loop: " << cycles;
	return 0;
}