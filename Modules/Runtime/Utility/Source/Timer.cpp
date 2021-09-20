#include "Timer.h"
#include <chrono>

namespace Ry
{
	
	Timer::Timer():
		delay(0.0),
		started(false)
	{
		start = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
	}

	Timer::Timer(double delay):
		delay(delay),
		started(false)
	{
		start = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
	}

	Timer::~Timer()
	{

	}

	void Timer::set_delay(double delay)
	{
		this->delay = delay;
	}

	void Timer::restart()
	{
		start = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
	}

	bool Timer::is_ready()
	{
		if (!started)
		{
			started = true;
			return false;
		}
		else
		{
			double now = (std::chrono::high_resolution_clock::now().time_since_epoch().count()) / 1e9;
			double delta = now - start;
			if (delta >= delay)
			{
				start = now;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}