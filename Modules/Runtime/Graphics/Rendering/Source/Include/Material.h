#pragma once

#include "Interface2/Texture2.h"
#include "RyMath.h"
#include "RenderingGen.h"

namespace Ry
{
	struct RENDERING_MODULE Material
	{
		Ry::String Name;
		Vector3 Ambient;
		Vector3 Diffuse;
		Vector3 Specular;

		Ry::Texture2* DiffuseTexture = nullptr;
	};
}
