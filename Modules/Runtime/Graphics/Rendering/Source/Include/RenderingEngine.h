#pragma once

#include "Asset.h"
#include "Core/Core.h"
#include "Core/String.h"
#include "Vertex.h"
#include "Data/Map.h"
#include "RenderPipeline.h"
#include "RenderingGen.h"

namespace Ry
{

	class RenderPipeline;
	class Texture2;

	/**
	 * Initializes rendering engine resources.
	 */
	RENDERING_MODULE void InitRenderingEngine();
	RENDERING_MODULE void QuitRenderingEngine();

	/**
	 * Global resize manager. Resizes framebuffers associated with global rendering passes.
	 */
	void HandleResize(int32 Width, int32 Height);

	extern RENDERING_MODULE Ry::Texture2* DefaultTexture;

	// Standard rendering passes
	extern RENDERING_MODULE class OffScreenRenderingPass* UIPass;
	extern RENDERING_MODULE class RenderingPass* TextPass;
	extern RENDERING_MODULE class ImposePass* UIImposePass;
	extern RENDERING_MODULE class OffScreenRenderingPass* ScenePass;
	extern RENDERING_MODULE class ImposePass* SceneImposePass;

	// Standard render pipelines
	extern RENDERING_MODULE UniquePtr<RenderPipeline> ObjectPipeline;

	// Standard batchers
	extern RENDERING_MODULE class Batch* ShapeBatcher;
	extern RENDERING_MODULE class Batch* TextBatcher;

	// Shader management
	class Shader2;
	extern Map<String, Ry::Shader2*> CompiledShaders;
	RENDERING_MODULE Shader2* CompileShader(const String& Name, Ry::String VertexLoc, Ry::String FragmentLoc);
	RENDERING_MODULE Shader2* GetShader(const String& Name);

}
