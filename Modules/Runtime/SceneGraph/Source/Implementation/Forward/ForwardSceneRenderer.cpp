#include "Forward/ForwardSceneRenderer.h"
#include "Application.h"
#include "Interface2/RenderAPI.h"
#include "SwapChain.h"
#include "Interface2/RenderingResource.h"
#include "RenderingEngine.h"
#include "Interface2/Pipeline.h"
#include "Interface2/RenderCommand.h"
#include "SceneGraph.h"
#include "ScenePrimitive.h"

namespace Ry
{

	ForwardSceneRenderer::ForwardSceneRenderer(Ry::SceneGraph* TarScene):
	SceneRenderer(TarScene)
	{
		this->StaticCmdBuffer = nullptr;
		this->DynamicCmdBuffer = nullptr;
	}

	void ForwardSceneRenderer::Init()
	{
		SceneRenderer::Init();

		// Create pipeline
		CreatePipeline();

		// Create command buffers
		// TODO: use own render pass, not default render pass of screen
		StaticCmdBuffer = Ry::NewRenderAPI->CreateCommandBuffer(Ry::app->GetSwapChain(), Ry::app->GetSwapChain()->GetDefaultRenderPass());
		DynamicCmdBuffer = Ry::NewRenderAPI->CreateCommandBuffer(Ry::app->GetSwapChain(), Ry::app->GetSwapChain()->GetDefaultRenderPass());

		// Setup callback whenever swapchain gets recreated so we can update renderpass
		Ry::app->GetSwapChain()->OnSwapChainDirty.AddMemberFunction(this, &ForwardSceneRenderer::OnSwapChainDirty);

		// Record static geometry
		RecordStatic();
		RecordDynamic();
	}

	void ForwardSceneRenderer::Render()
	{
		// Update scene resources
		SceneResources->SetMatConstant("Scene", "ViewProj", ViewProjectionMatrix);

		// Update primitive resources
		for(ScenePrimitive* Prim : TargetScene->GetPrimitives())
		{
			PrimitiveResources* AssocRes = *PrimResources.get(Prim);

			// Set the primitive's world transform
			AssocRes->GetPrimitiveSpecificResources()->SetMatConstant("Model", "Transform", Prim->GetWorldTransform().get_transform());

			// TODO: do we need to set material specific resources on frame?
		}
		
		// Record command buffers needed

		// TODO: figure out best way to split command buffers up
		RecordStatic();
		RecordDynamic();
	}

	RenderingCommandBuffer2* ForwardSceneRenderer::GetCmdBuffer()
	{
		return StaticCmdBuffer;
	}

	void ForwardSceneRenderer::OnSwapChainDirty()
	{
		StaticCmdBuffer->UpdateParentRenderPass(Ry::app->GetSwapChain()->GetDefaultRenderPass());
		DynamicCmdBuffer->UpdateParentRenderPass(Ry::app->GetSwapChain()->GetDefaultRenderPass());
	}
	
	void ForwardSceneRenderer::CreateResources()
	{
		// Create resource descriptions, and high level resources
		SceneResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Ry::ShaderStage::Vertex }, 0);
		SceneResDesc->AddConstantBuffer(0, "Scene", {
			DeclPrimitive(Float4x4, "ViewProj")
		});
		SceneResDesc->CreateDescription();

		PrimResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Ry::ShaderStage::Vertex }, 1);
		PrimResDesc->AddConstantBuffer(1, "Model", {
			DeclPrimitive(Float4x4, "Transform")
			});
		PrimResDesc->CreateDescription();

		LightResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Ry::ShaderStage::Fragment }, 2);
		LightResDesc->AddConstantBuffer(2, "Light", {
			DeclPrimitive(Float, "Intensity"),
			DeclPrimitive(Float3, "Color"),
			DeclPrimitive(Float3, "Direction")
		});
		LightResDesc->CreateDescription();

		MaterialDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Ry::ShaderStage::Fragment }, 3);
		MaterialDesc->AddConstantBuffer(3, "Mat", {
			DeclPrimitive(Float, "UseDiffuseTexture"),
			DeclPrimitive(Float3, "DiffuseColor"),
			DeclPrimitive(Float3, "AmbientColor"),
			DeclPrimitive(Float3, "SpecularColor")
		});
		MaterialDesc->AddTextureBinding(0, "Diffuse");
		MaterialDesc->CreateDescription();

		SceneResources = Ry::NewRenderAPI->CreateResourceSet(SceneResDesc, Ry::app->GetSwapChain());
		SceneResources->CreateBuffer();

		LightResources = Ry::NewRenderAPI->CreateResourceSet(LightResDesc, Ry::app->GetSwapChain());
		
		Ry::Vector3 Dir = { -0.5f, -0.5f, 0.0f };
		normalize(Dir);
		
		LightResources->SetFloatConstant("Light", "Intensity", 1.0f);
		LightResources->SetMatConstant("Light", "Color", Ry::Vector3(1.0f, 1.0f, 1.0f));
		LightResources->SetMatConstant("Light", "Direction", Dir);
		
		LightResources->CreateBuffer();
	}

	void ForwardSceneRenderer::CreatePipeline()
	{
		// Create and manage all pipelines associated with this renderer
		Ry::Shader2* LvShader = Ry::GetShader("Level");

		if (!LvShader)
		{
			Ry::Log->Log("Compiling level shader for first time");
			LvShader = Ry::CompileShader("Level", "Vertex/Level", "Fragment/Level");
		}

		// Create new pipeline
		Ry::PipelineCreateInfo CreateInfo;
		CreateInfo.ViewportWidth = Ry::app->GetSwapChain()->GetSwapChainWidth();
		CreateInfo.ViewportHeight = Ry::app->GetSwapChain()->GetSwapChainHeight();
		CreateInfo.PipelineShader = LvShader;
		CreateInfo.VertFormat = Ry::VF1P1UV1N;
		CreateInfo.RenderPass = Ry::app->GetSwapChain()->GetDefaultRenderPass();
		CreateInfo.Blend.bEnabled = true;
		CreateInfo.Depth.bEnableDepthTest = true;

		// Add resource description to pipeline
		CreateInfo.ResourceDescriptions.Add(SceneResDesc);
		CreateInfo.ResourceDescriptions.Add(PrimResDesc);
		CreateInfo.ResourceDescriptions.Add(LightResDesc);
		CreateInfo.ResourceDescriptions.Add(MaterialDesc);

		Pipeline = Ry::NewRenderAPI->CreatePipeline(CreateInfo);
		Pipeline->CreatePipeline();
	}

	void ForwardSceneRenderer::RecordStatic()
	{
		StaticCmdBuffer->Reset();

		StaticCmdBuffer->BeginCmd();
		{
			// Bind pipeline
			StaticCmdBuffer->BindPipeline(Pipeline);

			// Set the viewport/scissor size state for the pipeline
			StaticCmdBuffer->SetViewportSize(0, 0, (float)Ry::GetViewportWidth(), (float)Ry::GetViewportHeight());
			StaticCmdBuffer->SetScissorSize(0, 0, (float)Ry::GetViewportWidth(), (float)Ry::GetViewportHeight());

			// For each material in each primitive:
			// Bind its associated resources, and call draw
			for(ScenePrimitive* ScenePrim : TargetScene->GetPrimitives())
			{
				PrimitiveResources* PrimRes = *PrimResources.get(ScenePrim);
				PrimRes->RecordDraw(StaticCmdBuffer);
			}

		}

		StaticCmdBuffer->EndCmd();
	}

	void ForwardSceneRenderer::RecordDynamic()
	{

	}
	
}
