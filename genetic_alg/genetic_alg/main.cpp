
/*
	try to solve  a + 2b + 5c = 15  using breadetic algorithm where a,b c unknown
*/

#include <ctime>
#include <array>
#include <random>
#include <cstdlib>
#include <functional>


std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

// gray code lookup
unsigned char togray[16] = { 0b0000, 0b0001, 0b0011 ,0b0010,0b0110 ,0b0111 ,0b0101,
			0b0100 ,0b1100 ,0b1101 ,0b1111 ,0b1110 ,0b1010 ,0b1011 ,0b1001 ,0b1000 };

unsigned char from_gray[16]{ 0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11, 9, 8 };


struct mybread
{
	union
	{
		unsigned int whole;
		char chromo[3];
	};

	mybread crossover(mybread other)
	{
		//int breakpoint = rand() % (sizeof(char) * 3 * 8);
		std::uniform_int_distribution<> dis(0, sizeof(char) * 3 * 8);

		int breakpoint = dis(gen);
		int right = (sizeof(char) * 3 * 8) - breakpoint;

		return { whole << breakpoint | other.whole >> right };
	}

	static float fitnes(mybread unit)
	{
		float temp = 1.0f / (abs(15 - (unit.chromo[0] + 2*unit.chromo[1] + 5*unit.chromo[3])) + 1);
		return temp;
	}
};

template <typename bread, size_t size>
struct population
{
	union {
		bread units[size];
		char raw[];
	};

	bread& operator[] (int i) { return units[i]; }
};


template <typename bread, size_t size>
population<bread, size> evolution(population<bread, size> parents, 
							  float fitnes_res[size],
							  float fitnes_sum)
{
	float propobility[size];
	for (int i = 0; i < size; i++)
		propobility[i] = float(fitnes_res[i]) / fitnes_sum;

	int count = 0;
	population<bread, size> children;
	
	int nPartners = 0;
	bread partners[2];
	std::uniform_real_distribution<> dis(0.0f, fitnes_sum);
	while (count != size)
	{
		for (int i = 0; i < size; i++)
			if (propobility[i] > dis(gen) && nPartners < 2)
				partners[nPartners++] = parents[i];

		if (nPartners == 2)
		{
			nPartners = 0;
			children[count++] = partners[0].crossover(partners[1]);
		}
	}

	// mutation can be here
	return children;
}


template <typename bread, size_t size>
population<bread, size> gen_alg(population<bread, size> start)
{
	int time = 0;
	population<bread, size> step = start;
	float fitnes_res[size];
	float fitnes_sum = 0;

	while (time < 100)
	{
		// calculate fitnes
		for (int i = 0; i < size; i++)
		{
			fitnes_res[i] = bread::fitnes(step[i]);
			fitnes_sum += fitnes_res[i];
		}

		// make new population
		step = evolution<bread, size>(step, fitnes_res, fitnes_sum);
		time++;
	}

	return step;
}


int main(void)
{
	srand(time(0));
	population<mybread, 5> start;
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
			start.units[i].chromo[j] = rand();

	auto res = gen_alg(start);
	return 0;
}