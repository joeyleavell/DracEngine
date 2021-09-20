#pragma once

#include "Core/Core.h"
#include "UtilityGen.h"

namespace Ry
{
	class UTILITY_MODULE Timer
	{
	public:

		Timer();

		/**
		*
		* @param delay The amount of delay time in seconds.
		*/
		Timer(double delay);
		~Timer();

		void set_delay(double delay);

		bool is_ready();
		void restart();

	private:
		bool started;
		double delay;
		double start;
	};
}
