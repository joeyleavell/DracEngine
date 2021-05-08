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
	#define RefClass(...) Annotate(Reflect, __VA_ARGS__)
	#define RefField(...) Annotate(Reflect, __VA_ARGS__)
	#define RefFunction(...) Annotate(Reflect, __VA_ARGS__)
#else
	#define Annotate(...)
	#define RefClass(...)
	#define RefField(...)
	#define RefFunction(...)
#endif

/************************************************************************/
/* Include additional Graphics implementation                           */
/************************************************************************/
// OpenGL
// DirectX