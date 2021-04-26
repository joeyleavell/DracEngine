#pragma once

#include "SceneRenderer.h"
#include "Data/Map.h"

namespace Ry
{

	class RenderingCommandBuffer2;
	class ResourceSetDescription;
	class ResourceSet;
	class Pipeline2;
	class RenderPass2;
	class ScenePrimitive;

	class SCENEGRAPH_MODULE ForwardSceneRenderer : public SceneRenderer
	{
	public:

		ForwardSceneRenderer(Ry::SceneGraph* TarScene);

		void Init() override;
		void Render() override;
		RenderingCommandBuffer2* GetCmdBuffer() override;


	private:

		void OnSwapChainDirty();

		void CreateResources() override;

		void CreatePipeline();

		void RecordStatic();
		void RecordDynamic();

		// TODO: Since pipeline houses the shader,
		// there will most likely end up being many of these
		// For now, I've simplified it to just one pipeline
		Ry::Pipeline2* Pipeline;

		// The parent render pass 
		//Ry::RenderPass2* ParentPass;

		Ry::RenderingCommandBuffer2* DynamicCmdBuffer;
		Ry::RenderingCommandBuffer2* StaticCmdBuffer;

	};

}
