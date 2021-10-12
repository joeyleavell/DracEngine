#pragma once

#include "Core/Core.h"
#include "Vector.h"
#include "Matrix.h"
#include "Core/Object.h"
#include "RyMath.gen.h"

#define PI 3.14159f
#define DEG_TO_RAD(DEG) (DEG * (PI / 180.0f))
#define RAD_TO_DEG(RAD) (RAD * (180.0f / PI))
#define FP_TOLERANCE 0.001

namespace Ry
{
	
	/*class TestReflection : public Object
	{
	public:

		GeneratedBody()

		RefField()
		Ry::TestRefl* Other = new Ry::TestRefl;

		RefField()
		Ry::TestRefl* Other2 = new Ry::TestRefl;

		RefFunction()
		void TestFunction();
	}RefClass();*/

	bool IsNearlyEqual(float A, float B, float Threshold = 0.0001f);
}