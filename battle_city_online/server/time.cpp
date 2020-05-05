
#include <chrono>

using namespace std::chrono;

high_resolution_clock::time_point init_time;

float get_time()
{
	high_resolution_clock::time_point now = high_resolution_clock::now();
	duration<float>	dur = duration_cast<duration<float>>(now - init_time);
	return dur.count();
}



struct Timer
{
	high_resolution_clock::time_point privius;

	int FPS = 0;
	float FrameTimeAVG = 0.0f;

	float m_timeSum = 0.0f;
	int m_Frames = 0;

	bool m_frameLock;
	float m_frameLock_time = 0.016777f;

	float elapsed = 0.0f;


	inline Timer(bool frame_lock = false) : m_frameLock(frame_lock)
	{
		privius = high_resolution_clock::now();
	}

	inline void update()
	{
		high_resolution_clock::time_point now = high_resolution_clock::now();
		duration<float>	dur = duration_cast<duration<float>>(now - privius);
		privius = now;
		elapsed = dur.count();
		m_Frames++;

		if (m_frameLock && elapsed < m_frameLock_time)
		{
			float dif = m_frameLock_time - elapsed;
			elapsed = m_frameLock_time;
			std::this_thread::sleep_for(dur);
		}

		m_timeSum += elapsed;
		if (m_timeSum > 1.0f)
		{
			FPS = m_Frames;
			FrameTimeAVG = m_timeSum / FPS;

			m_timeSum = 0;
			m_Frames = 0;
		}
	}
};