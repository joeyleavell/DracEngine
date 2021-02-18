#pragma once

#include "Asset.h"
#include "Core/Core.h"
#include "Core/String.h"
#include "Vertex.h"
#include "Data/Map.h"
#include "RenderPipeline.h"
#include "RenderingCoreGen.h"

namespace Ry
{

	class RenderPipeline;

	/**
	 * Initializes rendering engine resources.
	 */
	RENDERINGCORE_MODULE void InitRenderingEngine();
	RENDERINGCORE_MODULE void QuitRenderingEngine();

	/**
	 * Global resize manager. Resizes framebuffers associated with global rendering passes.
	 */
	void HandleResize(int32 Width, int32 Height);

	// Standard rendering passes
	extern RENDERINGCORE_MODULE class OffScreenRenderingPass* UIPass;
	extern RENDERINGCORE_MODULE class RenderingPass* TextPass;
	extern RENDERINGCORE_MODULE class ImposePass* UIImposePass;
	extern RENDERINGCORE_MODULE class OffScreenRenderingPass* ScenePass;
	extern RENDERINGCORE_MODULE class ImposePass* SceneImposePass;

	// Standard render pipelines
	extern RENDERINGCORE_MODULE UniquePtr<RenderPipeline> ObjectPipeline;

	// Standard batchers
	extern RENDERINGCORE_MODULE class Batch* ShapeBatcher;
	extern RENDERINGCORE_MODULE class Batch* TextBatcher;

	// Shader management
	class Shader;
	extern Map<String, Ry::Shader*> CompiledShaders;
	RENDERINGCORE_MODULE Shader* CompileShader(const String& Name, const VertexFormat& Format, const AssetRef& VertexShader, const AssetRef& FragmentShader);
	RENDERINGCORE_MODULE Shader* GetShader(const String& Name);

}
