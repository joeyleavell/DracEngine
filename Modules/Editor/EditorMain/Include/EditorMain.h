#pragma once

#include "Window.h"
#include "RenderingEngine.h"
#include "GLRenderAPI.h"
#include "Manager/AssetManager.h"
#include "Factory/TextFileFactory.h"
#include "Factory/ObjMeshFactory.h"
#include "Factory/TextureFactory.h"
#include "Factory/AudioFactory.h"
#include "Factory/TrueTypeFontFactory.h"
#include "2D/Batch/Batch.h"
#include "Widget/UserInterface.h"
#include "Widget/BorderWidget.h"
#include "VulkanRenderAPI.h"
#include "Widget/VerticalPanel.h"
#include "Language/ShaderCompiler.h"
#include "Interface/RenderCommand.h"
#include "Interface/VertexArray.h"
#include "Interface/Pipeline.h"
#include <chrono>
#include "Mesh.h"
#include "MeshAsset.h"
#include "Transform.h"
#include "Material.h"
#include "Interface/RenderingResource.h"

namespace Ry
{
	class Editor2
	{
	public:

		ResourceSet* ResSet = nullptr;
		Ry::Float4x4 Proj;
		
		Editor2(Ry::RenderingPlatform Platform)
		{
			Ry::rplatform = new Ry::RenderingPlatform(Platform);
		}

		void InitFileSystem()
		{
			Ry::String EngineRoot = Ry::File::GetParentPath(Ry::File::GetParentPath(GetPlatformModulePath()));
			Ry::String ResourcesRoot = Ry::File::Join(EngineRoot, "Resources");
			Ry::String ShadersRoot = Ry::File::Join(EngineRoot, "Shaders");

			Ry::File::MountDirectory(ResourcesRoot, "Engine");

			// Todo: Support custom game shaders as well
			Ry::File::MountDirectory(ShadersRoot, "Shaders");
		}

		void InitAssetSystem()
		{
			AssetMan = new Ry::AssetManager;

			// Register asset factories
			// TODO: these should be moved
			AssetMan->RegisterFactory("text", new Ry::TextFileFactory);
			AssetMan->RegisterFactory("mesh/obj", new ObjMeshFactory);
			AssetMan->RegisterFactory("font/truetype", new TrueTypeFontFactory);
			AssetMan->RegisterFactory("image", new TextureFactory);
			AssetMan->RegisterFactory("sound", new AudioFactory);
		}

		int32 GetViewportWidth()
		{
			return EditorMainWindow->GetFramebufferWidth();
		}

		int32 GetViewportHeight()
		{
			return EditorMainWindow->GetFramebufferHeight();
		}

		void InitWindow()
		{
			EditorMainWindow = new Window(*Ry::rplatform);

			if (!EditorMainWindow->CreateWindow("Editor", 1080, 720))
			{
				Ry::Log->Log("Failed to create a window");
			}
			else
			{
				Ry::Log->Log("Window created");
			}

			Ry::ViewportWidthDel.BindMemberFunction(this, &Editor2::GetViewportWidth);
			Ry::ViewportHeightDel.BindMemberFunction(this, &Editor2::GetViewportHeight);
			
		}

		void InitRenderAPI()
		{
			
			if(*Ry::rplatform == Ry::RenderingPlatform::OpenGL)
			{
				if (!Ry::InitOGLRendering())
				{
					Ry::Log->LogError("Failed to initialize OpenGL");
				}
				else
				{
					Ry::Log->Log("Initialized OpenGL");
				}
			}

			if(*Ry::rplatform == Ry::RenderingPlatform::Vulkan)
			{
				if(!InitVulkanAPI())
				{
					Ry::Log->LogError("Failed to initialize Vulkan API");
					std::abort();
				}
			}

		}

		void ShutdownRenderAPI()
		{
			if (*Ry::rplatform == Ry::RenderingPlatform::Vulkan)
			{
				ShutdownVulkanAPI();
			}
		}

		void InitLogger()
		{
			Ry::Log = new Ry::Logger;
		}

		void InitUI()
		{
			if (*Ry::rplatform == Ry::RenderingPlatform::OpenGL)
			{
				UI = new UserInterface;

				// Ry::SharedPtr<Ry::BorderWidget> Canvas;
				// NewWidgetAssign(Canvas, BorderWidget)
				// .FillX(1.0f)
				// .FillY(1.0f)
				// .HorAlign(HAlign::CENTER)
				// .VertAlign(VAlign::BOTTOM)
				// [
				// 	NewWidget(Ry::BorderWidget)
				// 	.DefaultBox(GREEN, GREEN, 0, 0)
				// 	.Padding(100.0f, 100.0f)
				// ];


				//UI->AddRoot(*Canvas);
			}

		}

		void Run()
		{
			InitWindowing();
			InitRenderAPI();
			InitLogger();
			InitFileSystem();
			InitAssetSystem();
			InitWindow();

			EditorMainWindow->GetSwapChain()->OnSwapChainDirty.AddMemberFunction(this, &Editor2::OnResize);

			// Create 1x1 white texture as a default diffuse map
			Ry::Bitmap DefaultDiffuseBmp(1, 1, PixelStorage::FOUR_BYTE_RGBA);
			DefaultDiffuseBmp.SetPixel(0, 0, 0xFFFFFFFF);
			DefaultDiffuse = Ry::RendAPI->CreateTexture(TextureFiltering::Nearest);
			DefaultDiffuse->Data(&DefaultDiffuseBmp);

			Proj = Ry::perspective4(EditorMainWindow->GetFramebufferWidth() / (float)EditorMainWindow->GetFramebufferHeight(), 70.0f, 0.01f, 2500.0f);
			// Load mesh
			Ry::MeshAsset* Asset = Ry::AssetMan->LoadAsset<MeshAsset>("/Engine/Mesh/Rbarn15.obj", "mesh/obj");
			NewMesh = Asset->CreateRuntimeMesh();

			// Setup scene resource description
			SceneDesc = Ry::RendAPI->CreateResourceSetDescription({ ShaderStage::Vertex, ShaderStage::Fragment }, 0);
			SceneDesc->AddConstantBuffer(0, "Scene", {
				DeclPrimitive(Float4x4, "ViewProj"),
			});
			SceneDesc->CreateDescription();

			// Setup material description
			MatDesc = Ry::RendAPI->CreateResourceSetDescription({ ShaderStage::Vertex, ShaderStage::Fragment }, 1);
			MatDesc->AddConstantBuffer(1, "Material", {
				DeclPrimitive(Float, "UseDiffuseTexture"),
				DeclPrimitive(Float4x4, "DiffuseColor"),
				DeclPrimitive(Float4x4, "AmbientColor"),
				DeclPrimitive(Float4x4, "SpecularColor"),
			});
			MatDesc->AddTextureBinding(0, "Diffuse");
			MatDesc->CreateDescription();
						
			ResSet = Ry::RendAPI->CreateResourceSet(SceneDesc, EditorMainWindow->GetSwapChain());
			ResSet->CreateBuffer();

			MeshData* Data = new MeshData;
			Data->SetVertFormat(VF1P1UV1N);
			
			Data->AddVertex({
				-0.5, -0.5f, 0.0f,
				0.0f, 0.0f,
				0.0f, 0.0f, 1.0f
			});
			
			Data->AddVertex({
				0.0, 0.5f, 0.0f,
				0.0f, 0.0f,
				0.0f, 0.0f, 1.0f
			});
			
			Data->AddVertex({
				0.5, -0.5f, 0.0f,
				0.0f, 0.0f,
				0.0f, 0.0f, 1.0f
			});
			
			Data->AddIndexData({
				0, 1, 2
			});
			
			Triangle = new Mesh(Data);
			
			
			// Find a texture
			for(int32 MatIndex = 0; MatIndex < NewMesh->GetMeshData()->SectionCount; MatIndex++)
			{
				ResourceSet* MatSet = Ry::RendAPI->CreateResourceSet(MatDesc, EditorMainWindow->GetSwapChain());
				Material* Mat = NewMesh->GetMeshData()->GetMaterial(MatIndex);
				
				if(Mat->AlbedoMap)
				{
					MatSet->SetFloatConstant("Material", "UseDiffuseTexture", 1.0f);
					MatSet->BindTexture("Diffuse", Mat->AlbedoMap);
				}
				else
				{
					MatSet->SetFloatConstant("Material", "UseDiffuseTexture", 0.0f);
					MatSet->SetMatConstant("Material", "DiffuseColor", Mat->Albedo);
					MatSet->SetMatConstant("Material", "AmbientColor", Mat->Albedo);
					MatSet->SetMatConstant("Material", "SpecularColor", Mat->Albedo);
					MatSet->BindTexture("Diffuse", DefaultDiffuse);
				}
			
				
				MatSet->CreateBuffer();
			
				ResourceSets.insert(Mat->Name, new ResourceSet*[2]{ResSet, MatSet});
			
			}

			Shader = Ry::RendAPI->CreateShader("Vertex/Vulkan", "Fragment/Vulkan");

			// Create new pipeline
			Ry::PipelineCreateInfo CreateInfo;
			CreateInfo.ViewportWidth = EditorMainWindow->GetSwapChain()->GetSwapChainWidth();
			CreateInfo.ViewportHeight = EditorMainWindow->GetSwapChain()->GetSwapChainHeight();
			CreateInfo.PipelineShader = Shader;
			CreateInfo.VertFormat = Ry::VF1P1UV1N;
			CreateInfo.RenderPass = EditorMainWindow->GetSwapChain()->GetDefaultRenderPass();
			CreateInfo.ResourceDescriptions.Add(SceneDesc);
			CreateInfo.ResourceDescriptions.Add(MatDesc);
			Pipeline = Ry::RendAPI->CreatePipeline(CreateInfo);
			Pipeline->CreatePipeline();
			
			CommandBuffer = Ry::RendAPI->CreateCommandBuffer(EditorMainWindow->GetSwapChain());
			RecordCommands();
			
			// Initialize rendering engine
			// if(*Ry::rplatform == Ry::RenderingPlatform::OpenGL)
			// {
			// 	Ry::InitRenderingEngine();
			// 	InitUI();
			// }

			int32 FPS = 0;
			LastFps = std::chrono::high_resolution_clock::now();
			while (!EditorMainWindow->ShouldClose())
			{
				FPS++;
				UpdateEditor();
				RenderEditor();

				std::chrono::duration<double> Delta = std::chrono::high_resolution_clock::now() - LastFps;
				if(std::chrono::duration_cast<std::chrono::seconds>(Delta).count() >= 1.0)
				{
					LastFps = std::chrono::high_resolution_clock::now();
					Ry::Log->Logf("FPS: %d", FPS);
					FPS = 0;
				}
			}

			Cleanup();

			system("pause");
		}

		float Delta = 0.0f;

		void RecordCommands()
		{
			CommandBuffer->BeginCmd();
			{
				CommandBuffer->BeginRenderPass(true);
				{
					CommandBuffer->SetViewportSize(0, 0, EditorMainWindow->GetFramebufferWidth(), EditorMainWindow->GetFramebufferHeight());
					CommandBuffer->SetScissorSize(0, 0, EditorMainWindow->GetFramebufferWidth(), EditorMainWindow->GetFramebufferHeight());
					CommandBuffer->BindPipeline(Pipeline);

					//CommandBuffer->BindResources(&ResSet, 1);
					//CommandBuffer->DrawVertexArrayIndexed(Triangle->GetVertexArray(), 0, 3);
					for (int32 MatIndex = 0; MatIndex < NewMesh->GetMeshData()->SectionCount; MatIndex++)
					{
						Material* Mat = NewMesh->GetMeshData()->GetMaterial(MatIndex);

						//CommandBuffer->BindResources(&ResSet, 1);

						CommandBuffer->BindResources(*ResourceSets.get(Mat->Name), 2);
						CommandBuffer->DrawVertexArrayIndexed(NewMesh->GetVertexArray(), NewMesh->GetMeshData()->Sections.Get(MatIndex).StartIndex, NewMesh->GetMeshData()->Sections.Get(MatIndex).Count);
					}
				}
				CommandBuffer->EndRenderPass();

			}
			CommandBuffer->EndCmd();
		}

		void OnResize()
		{
			RecordCommands(); // Re record commands since viewport size has changed
		}

		void UpdateEditor()
		{
			EditorMainWindow->Update();

			Delta += 0.01f;

			Ry::Transform TestTransform;
			TestTransform.position = Ry::Vector3(0.f, 0.0f, 5.0f);

			Ry::Transform Model;
			Model.position = Ry::Vector3(0.0f, 0.0f, -1500.0f);
			Model.rotation = Ry::Vector3(0.0f, Delta, Delta);
			Model.scale = Ry::Vector3(1.0f, 1.0f, 1.0f);

			Ry::Transform Trans;
			Trans.position = Ry::Vector3(0.0f, 0.0f, 0.0f);
			Trans.rotation = Ry::Vector3(0.0f, 0.0f, 0.0f);
			Trans.scale = Ry::Vector3(1.0f, 1.0f, 1.0f);

			Ry::Float4x4 View = Trans.get_transform();
			ResSet->SetMatConstant("Scene", "ViewProj", Proj * View * Model.get_transform());

		}

		void RenderEditor()
		{
			// if (*Ry::rplatform == Ry::RenderingPlatform::OpenGL)
			// {
			// 	Ry::RenderAPI->BindDefaultFramebuffer();
			// 	Ry::RenderAPI->ClearBuffers(true, false);
			//
			// 	// Render here
			// 	UI->Render();
			// }
			
			EditorMainWindow->BeginFrame();
			{
				CommandBuffer->Submit();	
			}
			EditorMainWindow->EndFrame();
			
		}

		int32 Frame = 0;

		void Cleanup()
		{
			// Cleanup rendering resources

			// Unload all assets
 
			ResSet->DeleteBuffer();
			delete ResSet;

			Ry::KeyIterator<Ry::String, ResourceSet**> ResourceSetsItr = ResourceSets.CreateKeyIterator();
			while(ResourceSetsItr)
			{
				ResourceSet** Sets = *ResourceSetsItr.Value();
				
				Sets[1]->DeleteBuffer();
				delete Sets[1];
				
				++ResourceSetsItr;
			}

			SceneDesc->DeleteDescription();
			MatDesc->DeleteDescription();

			delete SceneDesc;
			delete MatDesc;

			DefaultDiffuse->DeleteTexture();
			delete DefaultDiffuse;

			Shader->DestroyShader();
			delete Shader;
			
			Pipeline->DeletePipeline();
			delete Pipeline;
			
			EditorMainWindow->Destroy();

			ShutdownRenderAPI();
		}

	private:

		std::chrono::time_point<std::chrono::high_resolution_clock> LastFps;
		Ry::Window* EditorMainWindow;
		Ry::Batch* UIBatch;
		Ry::UserInterface* UI;
		Ry::Pipeline* Pipeline;
		Ry::CommandBuffer* CommandBuffer;
		Ry::Shader* Shader;
		Ry::Mesh* NewMesh = nullptr;
		Ry::Mesh* Triangle = nullptr;

		Ry::Map<Ry::String, ResourceSet**> ResourceSets;
		Ry::Map<Ry::String, uint32> ResourceSetCount;

		Ry::ResourceLayout* SceneDesc;
		Ry::ResourceLayout* MatDesc;

		Ry::Texture* DefaultDiffuse;
	};

}
