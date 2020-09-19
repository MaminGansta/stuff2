#pragma once
#include <iostream>
#include <chrono>

namespace my
{
	using namespace std::chrono;

    class Timer
    {
        static high_resolution_clock::time_point s_ProgramStart;
        high_resolution_clock::time_point m_StartTime;
        
    public:
        Timer();
        ~Timer();

        // Time from the program start
		static float GetTime();

    private:
		static high_resolution_clock::time_point Now();
    };
   
}
