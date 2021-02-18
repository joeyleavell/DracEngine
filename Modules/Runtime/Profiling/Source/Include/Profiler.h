#pragma once

#include "Core/Core.h"
#include "ProfilingGen.h"

namespace Ry
{

	/**
	 * Provides functionality to time sections of code.
	 */
	class PROFILING_MODULE Profiler
	{

	public:

		Profiler();
		~Profiler();

		/**
		 * Begins recording time.
		 */
		void start();

		/**
		 * Stops recording time and stores the time difference since start() was called.
		 */
		void end();

		/**
		 * The duration between calling start() and end()
		 * @return float The time between start() and end() in milliseconds
		 */
		float get_duration_ms();

	private:

		uint64 start_time;
		int64 duration;
	};
}
