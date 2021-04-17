#pragma once

#include <ostream>
//#include "Memory/Memory.h"
#include "Core/Assert.h"
#include "Core/Log.h"
#include "Core/Platform.h"
#include "Core/Delegate.h"
#include "Core/String.h"
#include "Core/Memory/Memory.h"

// Reflection defines
// TODO: make these only defined for reflection generator, even if using clang
#ifdef __clang__
	#define Annotate(...) __attribute__ ((annotate(#__VA_ARGS__)))
	#define Class(...) Annotate(Reflect, __VA_ARGS__)
	#define Field(...) Annotate(Reflect, __VA_ARGS__)
	#define Function(...) Annotate(Reflect, __VA_ARGS__)
#else
	#define Annotate(...)
	#define Class(...)
	#define Field(...)
	#define Function(...)
#endif

/************************************************************************/
/* Include additional Graphics implementation                           */
/************************************************************************/
// OpenGL
// DirectX