
#include "timer.h"


namespace my
{
	high_resolution_clock::time_point Timer::s_ProgramStart = Now();

	Timer::Timer()
		: m_StartTime(high_resolution_clock::now())
	{}

	Timer::~Timer()
	{
		high_resolution_clock::time_point now = Now();
		duration<float> time_dif = duration_cast<duration<float>>(now - m_StartTime);
		std::cout << time_dif.count() << "s" << '\n';
	}

	std::chrono::high_resolution_clock::time_point Timer::Now()
	{
		return  high_resolution_clock::now();
	}

	float Timer::GetTime()
	{
		high_resolution_clock::time_point now = Now();
		duration<float> time_dif = duration_cast<duration<float>>(now - s_ProgramStart);
		return time_dif.count();
	}

}