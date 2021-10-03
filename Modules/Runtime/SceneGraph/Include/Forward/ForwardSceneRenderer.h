#pragma once

#include "SceneRenderer.h"
#include "Data/Map.h"
#include "ForwardSceneRenderer.gen.h"

namespace Ry
{

	class CommandBuffer;
	class ResourceLayout;
	class ResourceSet;
	class Pipeline;
	class RenderPass;
	class ScenePrimitive;
	class Shader;

	class SCENEGRAPH_MODULE ForwardSceneRenderer : public SceneRenderer
	{
	public:

		ForwardSceneRenderer(Ry::SceneGraph* TarScene);

		void Init() override;
		void Render() override;
		CommandBuffer* GetCmdBuffer() override;

		void RecordStatic();
		void RecordDynamic();

	private:

		Ry::Vector3 LightDir;

		void OnSwapChainDirty();

		void CreateResources() override;

		void CreatePipeline();


		// TODO: Since pipeline houses the shader,
		// there will most likely end up being many of these
		// For now, I've simplified it to just one pipeline
		Ry::Pipeline* Pipeline;
		Ry::Shader* Shader;

		// The parent render pass 
		//Ry::RenderPass* ParentPass;

		Ry::CommandBuffer* DynamicCmdBuffer;
		Ry::CommandBuffer* StaticCmdBuffer;

	};

}
