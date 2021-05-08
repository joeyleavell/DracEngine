#pragma once

#include "Interface/Texture.h"
#include "RyMath.h"
#include "RenderingGen.h"

namespace Ry
{
	struct RENDERING_MODULE Material
	{
		Ry::String Name;

		/**
		 * If the albedo map is null, this value is used instead.
		 */
		Vector3 Albedo = Vector3(1.0f, 1.0f, 1.0f);

		Vector3 Emissive = Vector3(0.0f, 0.0f, 0.0f);

		/**
		 * If the roughness map is null, this value is used instead.
		 */
		float Roughness = 0.01f;

		/**
		 * If the metallic map is null, this value is used instead.
		 */
		float Metallic = 0.0f;

		Ry::Texture* AlbedoMap    = nullptr;
		Ry::Texture* NormalMap    = nullptr;
		Ry::Texture* RoughnessMap = nullptr;
		Ry::Texture* MetallicMap  = nullptr;
		Ry::Texture* AOMap        = nullptr;
		Ry::Texture* EmissiveMap  = nullptr;

	};
}
