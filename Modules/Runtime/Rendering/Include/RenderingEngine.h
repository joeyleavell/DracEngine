#pragma once

#include "Core/String.h"
#include "Data/Map.h"
#include "RenderingGen.h"

namespace Ry
{
	class Shader;
	class RenderPipeline;
	class Texture;

	extern RENDERING_MODULE Ry::Texture* DefaultTexture;
	extern Map<String, Ry::Shader*> CompiledShaders; // Manages shaders

	/**
	 * Initializes rendering engine resources.
	 */
	RENDERING_MODULE void InitRenderingEngine();
	RENDERING_MODULE void QuitRenderingEngine();

	RENDERING_MODULE Shader* GetOrCompileShader(const String& Name, Ry::String Vertex, Ry::String Fragment);
	RENDERING_MODULE Shader* CompileShader(const String& Name, Ry::String VertexLoc, Ry::String FragmentLoc);
	RENDERING_MODULE Shader* GetShader(const String& Name);

}
