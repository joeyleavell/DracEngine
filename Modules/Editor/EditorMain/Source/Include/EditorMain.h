#include "Window.h"
#include "RenderingEngine.h"
#include "GLRendering.h"
#include "Manager/AssetManager.h"
#include "Factory/TextFileFactory.h"
#include "Factory/ObjMeshFactory.h"
#include "Factory/TextureFactory.h"
#include "Factory/AudioFactory.h"
#include "Factory/TrueTypeFontFactory.h"
#include "2D/Batch/Batch.h"
#include "Widget/UserInterface.h"
#include "Widget/BoxWidget.h"
#include "Widget/VerticalPanel.h"

namespace Ry
{
	class Editor
	{
	public:

		Editor()
		{

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
			EditorMainWindow = new Window;

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
			EditorMainWindow->InitContext();

			if (!Ry::InitOGLRendering())
			{
				Ry::Log->LogError("Failed to initialize OpenGL");
			}
			else
			{
				Ry::Log->Log("Initialized OpenGL");
			}
			
		}

		void InitLogger()
		{
			Ry::Log = new Ry::Logger;
		}

		void InitUI()
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

		void Run()
		{

			InitLogger();
			InitWindow();
			InitRenderAPI();
			InitFileSystem();
			InitAssetSystem();

			// Initialize rendering engine
			Ry::InitRenderingEngine();

			InitUI();

			while (!EditorMainWindow->ShouldClose())
			{
				UpdateEditor();
				RenderEditor();
			}

			EditorMainWindow->Destroy();
		}

		void UpdateEditor()
		{
			EditorMainWindow->Update();
		}

		void RenderEditor()
		{
			Ry::RenderAPI->BindDefaultFramebuffer();
			Ry::RenderAPI->ClearBuffers(true, false);
			
			// Render here
			UI->Render();

			EditorMainWindow->SwapBuffers();
		}

	private:
		Ry::Window* EditorMainWindow;
		Ry::Batch* UIBatch;
		Ry::UserInterface* UI;
	};

}
