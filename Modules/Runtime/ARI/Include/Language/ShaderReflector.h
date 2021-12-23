#pragma once

#include "Core/String.h"
#include "RenderingResource.h"

namespace Ry
{
	enum class ShaderStage;
	class String;

	void ReflectShader(Ry::String ShaderLoc, Ry::ShaderStage Stage, ShaderReflection& OutReflectionData);
	
}
