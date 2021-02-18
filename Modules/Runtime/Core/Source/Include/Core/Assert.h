#pragma once

/************************************************************************/
/* Asserts                                                               */
/************************************************************************/
#ifdef RY_DEBUG
#define CORE_ASSERT(cond, ...) \
	if(!(cond)) \
	{ \
		printf(__VA_ARGS__); \
		abort(); \
	}
#else
	#define CORE_ASSERT(cond, ...)
#endif
