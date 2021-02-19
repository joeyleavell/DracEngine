#pragma once

#include "Core/Core.h"
#include "Interface2/Shader2.h"
#include "RenderingGen.h"

namespace Ry
{

	bool RENDERING_MODULE HLSLtoSPIRV(const Ry::String& Source, uint8*& OutSpirV, int32& OutSize, Ry::String& ErrWarnMsg, ShaderStage Stage);
	bool RENDERING_MODULE HLSLtoGLSL(const Ry::String& Source, Ry::String& OutSource, Ry::String& ErrWarnMsg, ShaderStage Stage);

	
}