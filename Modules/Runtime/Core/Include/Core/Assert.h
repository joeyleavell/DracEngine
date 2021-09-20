#pragma once

/************************************************************************/
/* Asserts                                                               */
/************************************************************************/
#ifndef RY_DEBUG
#define CORE_ASSERT(cond) \
if(!(cond)) \
{ \
	printf(#cond); \
	abort(); \
}
#define CORE_ASSERTF(cond, ...) \
if(!(cond)) \
{ \
	printf(__VA_ARGS__); \
	abort(); \
}
#else
	#define CORE_ASSERT(cond, ...)
#endif
