#include "Timer.h"
#include <chrono>

namespace Ry
{
	
	Timer::Timer():
		started(false),
		delay(0.0),
		start(0.0)
	{
		start = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
	}

	Timer::Timer(double delay):
		started(false),
		delay(delay),
		start(0.0)
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