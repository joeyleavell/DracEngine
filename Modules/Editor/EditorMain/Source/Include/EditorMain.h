#pragma once

#include "Window.h"
#include "RenderingEngine.h"
#include "GLRenderAPI2.h"
#include "Manager/AssetManager.h"
#include "Factory/TextFileFactory.h"
#include "Factory/ObjMeshFactory.h"
#include "Factory/TextureFactory.h"
#include "Factory/AudioFactory.h"
#include "Factory/TrueTypeFontFactory.h"
#include "2D/Batch/Batch.h"
#include "Widget/UserInterface.h"
#include "Widget/BoxWidget.h"
#include "VulkanRenderAPI.h"
#include "Widget/VerticalPanel.h"
#include "Language/ShaderCompiler.h"
#include "Interface2/RenderCommand.h"
#include "Interface2/VertexArray2.h"
#include "Interface2/Pipeline.h"
#include <chrono>
#include "Mesh2.h"
#include "MeshAsset.h"
#include "Transform.h"
#include "Material.h"
#include "Interface2/RenderingResource.h"

namespace Ry
{
	class Editor
	{
	public:

		ResourceSet* ResSet = nullptr;
		Ry::Float4x4 Proj;
		
		Editor(Ry::RenderingPlatform Platform)
		{
			Ry::rplatform = new Ry::RenderingPlatform(Platform);
		}

		void InitFileSystem()
		{
			Ry::String EngineRoot = Ry::File::GetParentPath(Ry::File::GetParentPath(GetPlatformModulePath()));
			Ry::String ResourcesRoot = Ry::File::Join(EngineRoot, "Resources");

			Ry::File::MountDirectory(ResourcesRoot, "Engine");
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
			return EditorMainWindow->GetWindowWidth();
		}

		int32 GetViewportHeight()
		{
			return EditorMainWindow->GetWindowHeight();
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

			Ry::ViewportWidthDel.BindMemberFunction(this, &Editor::GetViewportWidth);
			Ry::ViewportHeightDel.BindMemberFunction(this, &Editor::GetViewportHeight);
			
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

				Ry::BoxWidget* Canvas;
				NewWidgetAssign(Canvas, BoxWidget)
					.FillX(1.0f)
					.FillY(1.0f)
					.SetHAlign(HAlign::CENTER)
					.SetVAlign(VAlign::BOTTOM)
					[
						NewWidget(Ry::BoxWidget)
						.Padding(100.0f, 100.0f)
						.DefaultBox(GREEN, GREEN, 0, 0)
					];


				UI->AddRoot(*Canvas);
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

			EditorMainWindow->GetSwapChain()->OnSwapChainDirty.AddMemberFunction(this, &Editor::OnResize);

			// Create 1x1 white texture as a default diffuse map
			Ry::Bitmap DefaultDiffuseBmp(1, 1, PixelStorage::FOUR_BYTE_RGBA);
			DefaultDiffuseBmp.SetPixel(0, 0, 0xFFFFFFFF);
			DefaultDiffuse = Ry::NewRenderAPI->CreateTexture();
			DefaultDiffuse->Data(&DefaultDiffuseBmp);

			Proj = Ry::perspective4(EditorMainWindow->GetWindowWidth() / (float)EditorMainWindow->GetWindowHeight(), 70.0f, 0.01f, 2500.0f);
			// Load mesh
			Ry::MeshAsset* Asset = dynamic_cast<MeshAsset*>(Ry::AssetMan->LoadAsset("/Engine/Mesh/Rbarn15.obj", "mesh/obj"));
			NewMesh = Asset->CreateRuntimeMesh();

			// Setup scene resource description
			SceneDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ ShaderStage::Vertex, ShaderStage::Fragment }, 0);
			SceneDesc->AddConstantBuffer(0, "Scene", {
				DeclPrimitive(Float4x4, "ViewProj"),
			});
			SceneDesc->CreateDescription();

			// Setup material description
			MatDesc = Ry::NewRenderAPI->CreateResourceSetDescription({ ShaderStage::Vertex, ShaderStage::Fragment }, 1);
			MatDesc->AddConstantBuffer(1, "Material", {
				DeclPrimitive(Float, "UseDiffuseTexture"),
				DeclPrimitive(Float4x4, "DiffuseColor"),
				DeclPrimitive(Float4x4, "AmbientColor"),
				DeclPrimitive(Float4x4, "SpecularColor"),
			});
			MatDesc->AddTextureBinding(0, "Diffuse");
			MatDesc->CreateDescription();
						
			ResSet = Ry::NewRenderAPI->CreateResourceSet(SceneDesc, EditorMainWindow->GetSwapChain());
			ResSet->CreateBuffer();

			MeshData* Data = new MeshData;
			Data->Format = VF1P1UV1N;
			
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
			
			Triangle = new Mesh2(Data);
			
			
			// Find a texture
			for(int32 MatIndex = 0; MatIndex < NewMesh->GetMeshData()->SectionCount; MatIndex++)
			{
				ResourceSet* MatSet = Ry::NewRenderAPI->CreateResourceSet(MatDesc, EditorMainWindow->GetSwapChain());
				Material* Mat = NewMesh->GetMeshData()->GetMaterial(MatIndex);
				
				if(Mat->DiffuseTexture)
				{
					MatSet->SetFloatConstant("Material", "UseDiffuseTexture", 1.0f);
					MatSet->BindTexture("Diffuse", Mat->DiffuseTexture);
				}
				else
				{
					MatSet->SetFloatConstant("Material", "UseDiffuseTexture", 0.0f);
					MatSet->SetMatConstant("Material", "DiffuseColor", Mat->Diffuse);
					MatSet->SetMatConstant("Material", "AmbientColor", Mat->Ambient);
					MatSet->SetMatConstant("Material", "SpecularColor", Mat->Diffuse);
					MatSet->BindTexture("Diffuse", DefaultDiffuse);
				}
			
				
				MatSet->CreateBuffer();
			
				ResourceSets.insert(Mat->Name, new ResourceSet*[]{ResSet, MatSet});
			
			}

			Shader = Ry::NewRenderAPI->CreateShader("/Engine/Shaders/Vertex/Vulkan.hlsl", "/Engine/Shaders/Fragment/Vulkan.hlsl");

			// Create new pipeline
			Ry::PipelineCreateInfo CreateInfo;
			CreateInfo.ViewportWidth = EditorMainWindow->GetSwapChain()->GetSwapChainWidth();
			CreateInfo.ViewportHeight = EditorMainWindow->GetSwapChain()->GetSwapChainHeight();
			CreateInfo.PipelineShader = Shader;
			CreateInfo.VertFormat = Ry::VF1P1UV1N;
			CreateInfo.RenderPass = EditorMainWindow->GetSwapChain()->GetDefaultRenderPass();
			CreateInfo.ResourceDescriptions.Add(SceneDesc);
			CreateInfo.ResourceDescriptions.Add(MatDesc);
			Pipeline = Ry::NewRenderAPI->CreatePipeline(CreateInfo);
			Pipeline->CreatePipeline();
			
			CommandBuffer = Ry::NewRenderAPI->CreateCommandBuffer(EditorMainWindow->GetSwapChain());
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
				CommandBuffer->BeginRenderPass();
				{
					CommandBuffer->SetViewportSize(0, 0, EditorMainWindow->GetWindowWidth(), EditorMainWindow->GetWindowHeight());
					CommandBuffer->SetScissorSize(0, 0, EditorMainWindow->GetWindowWidth(), EditorMainWindow->GetWindowHeight());
					CommandBuffer->BindPipeline(Pipeline);

					//CommandBuffer->BindResources(&ResSet, 1);
					//CommandBuffer->DrawVertexArrayIndexed(Triangle->GetVertexArray(), 0, 3);
					for (int32 MatIndex = 0; MatIndex < NewMesh->GetMeshData()->SectionCount; MatIndex++)
					{
						Material* Mat = NewMesh->GetMeshData()->GetMaterial(MatIndex);

						//CommandBuffer->BindResources(&ResSet, 1);

						CommandBuffer->BindResources(*ResourceSets.get(Mat->Name), 2);
						CommandBuffer->DrawVertexArrayIndexed(NewMesh->GetVertexArray(), NewMesh->GetMeshData()->Sections.get(MatIndex)->StartIndex, NewMesh->GetMeshData()->Sections.get(MatIndex)->Count);
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
		Ry::Pipeline2* Pipeline;
		Ry::RenderingCommandBuffer2* CommandBuffer;
		Ry::Shader2* Shader;
		Ry::Mesh2* NewMesh = nullptr;
		Ry::Mesh2* Triangle = nullptr;

		Ry::Map<Ry::String, ResourceSet**> ResourceSets;
		Ry::Map<Ry::String, uint32> ResourceSetCount;

		Ry::ResourceSetDescription* SceneDesc;
		Ry::ResourceSetDescription* MatDesc;

		Ry::Texture2* DefaultDiffuse;
	};

}
