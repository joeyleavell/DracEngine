#include "Forward/ForwardSceneRenderer.h"
#include "Application.h"
#include "Interface/RenderAPI.h"
#include "SwapChain.h"
#include "Interface/RenderingResource.h"
#include "RenderingEngine.h"
#include "Interface/Pipeline.h"
#include "Interface/RenderCommand.h"
#include "SceneGraph.h"
#include "ScenePrimitive.h"
#include "Interface/Shader.h"

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
		StaticCmdBuffer = Ry::RendAPI->CreateCommandBuffer(Ry::app->GetSwapChain(), Ry::app->GetSwapChain()->GetDefaultRenderPass());
		DynamicCmdBuffer = Ry::RendAPI->CreateCommandBuffer(Ry::app->GetSwapChain(), Ry::app->GetSwapChain()->GetDefaultRenderPass());

		// Setup callback whenever swapchain gets recreated so we can update renderpass
		Ry::app->GetSwapChain()->OnSwapChainDirty.AddMemberFunction(this, &ForwardSceneRenderer::OnSwapChainDirty);

		// Record static geometry
		RecordStatic();
		RecordDynamic();
	}

	void ForwardSceneRenderer::Render()
	{
		// Update scene resources
		SceneResources->SetMatConstant("Scene", "ViewProjection", ViewProjectionMatrix);

		// Set view position from inverse transform
		LightResources->SetMatConstant("SceneLighting", "ViewPosition", ViewPos);

		LightDir = LightDir.RotateVector(Ry::UpVector, 0.05f);
		LightResources->SetMatConstant("SceneLighting", "PointLights[0].Position", LightDir);

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
		//RecordStatic();
		//RecordDynamic();
	}

	CommandBuffer* ForwardSceneRenderer::GetCmdBuffer()
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
		Shader = Ry::GetOrCompileShader("Level", "Vertex/BlinnPhong", "Fragment/BlinnPhong");
		
		// Create resource descriptions, and high level resources
		SceneResDesc = Shader->GetVertexReflectionData()[0];
		PrimResDesc = Shader->GetVertexReflectionData()[1];
		LightResDesc = Shader->GetFragmentReflectionData()[2];
		MaterialDesc = Shader->GetFragmentReflectionData()[3];

		// Ry::ResourceLayout* SceneResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Ry::ShaderStage::Vertex }, 1);
		// SceneResDesc->AddConstantBuffer(1, "Scene", {
		// 	DeclPrimitive(Float4x4, "ViewProjection")
		// 	});
		// SceneResDesc->CreateDescription();
		// this->SceneResDesc = SceneResDesc;
		//
		//
		// Ry::ResourceLayout* PrimResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Ry::ShaderStage::Vertex }, 1);
		// PrimResDesc->AddConstantBuffer(1, "Model", {
		// 	DeclPrimitive(Float4x4, "Transform")
		// });
		// PrimResDesc->CreateDescription();
		// this->PrimResDesc = PrimResDesc;
		//
		// Ry::ResourceLayout* LightResDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Ry::ShaderStage::Fragment }, 2);
		// LightResDesc->AddConstantBuffer(2, "Light", {
		// 	DeclPrimitive(Float, "Intensity"),
		// 	DeclPrimitive(Float3, "Color"),
		// 	DeclPrimitive(Float3, "Direction")
		// });
		// LightResDesc->CreateDescription();
		// this->LightResDesc = LightResDesc;
		//
		// Ry::ResourceLayout* MaterialDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ Ry::ShaderStage::Fragment }, 3);
		// MaterialDesc->AddConstantBuffer(3, "Material", {
		// 	DeclPrimitive(Float, "UseDiffuseTexture"),
		// 	DeclPrimitive(Float3, "DiffuseColor"),
		// 	DeclPrimitive(Float3, "AmbientColor"),
		// 	DeclPrimitive(Float3, "SpecularColor")
		// });
		// MaterialDesc->AddTextureBinding(0, "DiffuseTexture");
		// MaterialDesc->CreateDescription();
		// this->MaterialDesc = MaterialDesc;
		

		SceneResources = Ry::RendAPI->CreateResourceSet(SceneResDesc, Ry::app->GetSwapChain());
		SceneResources->CreateBuffer();

		LightResources = Ry::RendAPI->CreateResourceSet(LightResDesc, Ry::app->GetSwapChain());
		{
			LightDir = { -0.5f, -0.5f, 0.0f };
			normalize(LightDir);

			LightResources->SetFloatConstant("SceneLighting", "NumPointLights", 7.0f);

			Ry::Vector3 Colors[] = {
				Ry::Vector3(1.0f, 1.0f, 1.0f),
				Ry::Vector3(0.0f, 1.0f, 0.0f),
				Ry::Vector3(0.0f, 1.0f, 1.0f),
				Ry::Vector3(1.0f, 0.0f, 0.0f),
				Ry::Vector3(1.0f, 0.0f, 1.0f),
				Ry::Vector3(1.0f, 1.0f, 0.0f),
				Ry::Vector3(1.0f, 1.0f, 1.0f),
			};
			
			for(int light = 0; light < 7; light++)
			{
				Float3 Forward = Ry::ForwardVector.RotatedVector(UpVector, 360.0f / 7 * light);
				Forward.y = -0.2f;

				Ry::String Light = "PointLights[" + Ry::to_string(light) + "]";
				
				LightResources->SetMatConstant("SceneLighting", Light + ".Position", Forward);
				LightResources->SetMatConstant("SceneLighting", Light + ".Color", Colors[light]);
			}


			// LightResources->SetMatConstant("SceneLighting", "PointLights[1].Position", LightDir);
			// LightResources->SetMatConstant("SceneLighting", "PointLights[1].Color", Ry::Vector3(0.0f, 0.0f,1.0f));
			//
			// LightResources->SetMatConstant("SceneLighting", "PointLights[2].Position", Ry::Vector3(0.0f, 0.5f, 1.0f));
			// LightResources->SetMatConstant("SceneLighting", "PointLights[2].Color", Ry::Vector3(1.0f, 0.0f, 0.0f));
			//
			// LightResources->SetMatConstant("SceneLighting", "PointLights[3].Position", Ry::Vector3(-0.5f, 0.5f, 1.0f));
			// LightResources->SetMatConstant("SceneLighting", "PointLights[3].Color", Ry::Vector3(1.0f, 0.0f, 1.0f));

		}
		LightResources->CreateBuffer();

	}

	void ForwardSceneRenderer::CreatePipeline()
	{
		// Create and manage all pipelines associated with this renderer
		Shader = Ry::GetOrCompileShader("Level", "Vertex/BlinnPhong", "Fragment/BlinnPhong");

		// Create new pipeline
		Ry::PipelineCreateInfo CreateInfo;
		CreateInfo.ViewportWidth = Ry::app->GetSwapChain()->GetSwapChainWidth();
		CreateInfo.ViewportHeight = Ry::app->GetSwapChain()->GetSwapChainHeight();
		CreateInfo.PipelineShader = Shader;
		CreateInfo.RenderPass = Ry::app->GetSwapChain()->GetDefaultRenderPass();
		CreateInfo.Blend.bEnabled = true;
		CreateInfo.Depth.bEnableDepthTest = true;

		Pipeline = Ry::RendAPI->CreatePipelineFromShader(CreateInfo, Shader);
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
