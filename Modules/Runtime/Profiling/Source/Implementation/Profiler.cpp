#include "Profiler.h"
#include <chrono>

namespace Ry
{
	
	Profiler::Profiler()
	{
	
	}
	
	Profiler::~Profiler()
	{
	
	}
	
	void Profiler::start()
	{
		start_time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	}
	
	void Profiler::end()
	{
		duration = std::chrono::high_resolution_clock::now().time_since_epoch().count() - start_time;
	}
	
	float Profiler::get_duration_ms()
	{
		return (duration / 1e6f);
	}
}
