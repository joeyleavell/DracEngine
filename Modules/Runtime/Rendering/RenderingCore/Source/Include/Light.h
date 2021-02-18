#pragma once

#include "Core/Core.h"
#include "Vector.h"
#include "Color.h"
#include "RenderingCoreGen.h"

namespace Ry
{

	struct RENDERINGCORE_MODULE DirectionalLight
	{
		Ry::Vector3 Direction;
		Ry::Color Color;
		float Intensity;

		DirectionalLight():
		Intensity(1.0f)
		{
			
		}
	};

	class RENDERINGCORE_MODULE SceneLighting
	{
	public:
		DirectionalLight Light;
	};
	
}
