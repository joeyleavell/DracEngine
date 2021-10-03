#include "RyMath.h"

namespace Ry
{
	/*Ry::Matrix4 solve_system(Ry::Vector4 a[12], Ry::Vector4 b[12])
	{

	}*/

	bool IsNearlyEqual(float A, float B, float Threshold)
	{
		return std::abs(A - B) <= Threshold;
	}
}
