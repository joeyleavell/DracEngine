#pragma once

#include "Core/Core.h"
#include "Vector.h"
#include "Color.h"
#include "RenderingGen.h"

namespace Ry
{

	struct RENDERING_MODULE DirectionalLight
	{
		Ry::Vector3 Direction;
		Ry::Color Color;
		float Intensity;

		DirectionalLight():
		Intensity(1.0f)
		{
			
		}
	};

	class RENDERING_MODULE SceneLighting
	{
	public:
		DirectionalLight Light;
	};
	
}
