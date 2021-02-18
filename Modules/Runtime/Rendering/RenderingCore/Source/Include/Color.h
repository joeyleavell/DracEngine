#pragma once

#include "Core/Core.h"
#include "Vector.h"
#include "RenderingCoreGen.h"

namespace Ry
{

	class RENDERINGCORE_MODULE Color
	{
	public:
		float Red;
		float Green;
		float Blue;
		float Alpha;

		Color():
		Color(0xFFFFFFFF)
		{
			
		}

		Color(int32 Hex)
		{
			this->Alpha = ((Hex >> 24) & 0xFF) / 255.0f;
			this->Red = ((Hex >> 16) & 0xFF) / 255.0f;
			this->Green = ((Hex >> 8) & 0xFF) / 255.0f;
			this->Blue = (Hex & 0xFF) / 255.0f;
		}

		Color(int32 R, int32 G, int32 B, int32 A)
		{
			this->Red = R / 255.0f;
			this->Green = G / 255.0f;
			this->Blue = B / 255.0f;
			this->Alpha = A / 255.0f;
		}

		Color(float R, float G, float B, float A)
		{
			this->Red = R;
			this->Green = G;
			this->Blue = B;
			this->Alpha = A;
		}

		Color ScaleRGB(float Amount) const
		{
			return Color(Red * Amount, Green * Amount, Blue * Amount, Alpha);
		}

		Ry::Vector4 ToVector() const
		{
			return Ry::Vector4(Red, Green, Blue, Alpha);
		}

		Ry::Vector3 ToVector3() const
		{
			return Ry::Vector3(Red, Green, Blue);
		}

	};

	static const Color RED = Color(0xFFFF0000);
	static const Color GREEN = Color(0xFF00FF00);
	static const Color BLUE = Color(0xFF0000FF);
	static const Color WHITE = Color(0xFFFFFFFF);
	static const Color BLACK = Color(0xFF000000);

}
