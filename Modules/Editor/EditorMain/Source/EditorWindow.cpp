#include "EditorWindow.h"
#include "Core/Globals.h"
#include "Interface/RenderAPI.h"
#include "Interface/RenderCommand.h"
#include "SwapChain.h"
#include "Event.h"
#include "EditorUI.h"
#include "Buttons.h"
#include "Keys.h"

namespace Ry
{
	EditorWindow::EditorWindow()
	{
		EdWindow = nullptr;
	}

	EditorWindow::~EditorWindow()
	{
		
	}

	int32 EditorWindow::GetViewportWidth()
	{
		return EdWindow->GetWindowWidth();
	}

	int32 EditorWindow::GetViewportHeight()
	{
		return EdWindow->GetWindowHeight();
	}

	void EditorWindow::InitWindow()
	{
		EdWindow = new Window(*Ry::rplatform);

		if (!EdWindow->CreateWindow("Editor", 1080, 720))
		{
			Ry::Log->Log("Failed to create a window");
		}
		else
		{
			Ry::Log->Log("Window created");
		}

		Ry::ViewportWidthDel.BindMemberFunction(this, &EditorWindow::GetViewportWidth);
		Ry::ViewportHeightDel.BindMemberFunction(this, &EditorWindow::GetViewportHeight);
	}

	void EditorWindow::Init()
	{
		InitWindow();
		Ry::InitRenderingEngine(); // Init rendering engine after init'ing window

		// Init editor UI layer
		EditorUI* UILayer = new EditorUI(EdWindow->GetSwapChain());
		EdLayers.PushLayer(UILayer);
		
		Delegate<void, int32, int32> Resized;
		Resized.BindMemberFunction(this, &EditorWindow::Resize);
		EdWindow->AddWindowResizedDelegate(Resized);
		//OnWindowResize.AddMemberFunction();

		EdWindow->OnEvent.AddMemberFunction(this, &EditorWindow::OnEvent);
	}

	void EditorWindow::Update(float Delta)
	{
		EdWindow->Update();

		// Update the layers
		EdLayers.Update(Delta);

	}
	
	void EditorWindow::Render()
	{
		EdWindow->BeginFrame();
		{
			// Render all of the layers
			EdLayers.Render();
		}
		EdWindow->EndFrame();
	}

	bool EditorWindow::WantsClose()
	{
		return EdWindow->ShouldClose();
	}

	void EditorWindow::Resize(int32 Width, int32 Height)
	{		
		EdLayers.OnResize(Width, Height);
	}

	void EditorWindow::OnEvent(const Event& Ev)
	{
		EdLayers.OnEvent(Ev);
	}
}
