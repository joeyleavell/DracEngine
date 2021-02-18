#pragma once

#include "Interface/Texture.h"
#include "RyMath.h"
#include "RenderingCoreGen.h"

namespace Ry
{
	struct RENDERINGCORE_MODULE Material
	{
		Ry::String Name;
		Vector3 Ambient;
		Vector3 Diffuse;
		Vector3 Specular;

		Ry::Texture* DiffuseTexture = nullptr;
	};
}
