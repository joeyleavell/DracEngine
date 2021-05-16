#include "EditorWindow.h"
#include "Core/Globals.h"
#include "Interface/RenderAPI.h"
#include "Interface/RenderCommand.h"
#include "SwapChain.h"
#include "Event.h"
#include "EditorUI.h"

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

		// Init editor UI layer
		EditorUI* UILayer = new EditorUI(EdWindow->GetSwapChain());
		EdLayers.PushLayer(UILayer);
	}

	void EditorWindow::Init()
	{
		Ry::InitRenderingEngine(); // Init rendering engine after init'ing window
		InitWindow();

		Delegate<void, int32, int32> Resized;
		Resized.BindMemberFunction(this, &EditorWindow::Resize);
		EdWindow->AddWindowResizedDelegate(Resized);
		//OnWindowResize.AddMemberFunction();


		Ry::Delegate<void, int, bool> ButtonPressedDelegate;
		ButtonPressedDelegate.BindMemberFunction(this, &EditorWindow::OnButtonPressed);
		EdWindow->AddMouseButtonDelegate(ButtonPressedDelegate);

		Ry::Delegate<void, uint32> KeyCharDelegate;
		KeyCharDelegate.BindMemberFunction(this, &EditorWindow::OnKeyChar);
		EdWindow->AddKeyCharDelegate(KeyCharDelegate);

		Ry::Delegate<void, int32, bool> KeyPressDelegate;
		KeyPressDelegate.BindMemberFunction(this, &EditorWindow::OnKeyPressed);
		EdWindow->AddKeyPressDelegate(KeyPressDelegate);

		Ry::Delegate<void, double, double> ScrollDelegate;
		ScrollDelegate.BindMemberFunction(this, &EditorWindow::OnScroll);
		EdWindow->AddScrollDelegate(ScrollDelegate);
	}

	void EditorWindow::Update()
	{
		// Update the layers
		// todo: use real delta time
		EdLayers.Update(0.0f);

		// Send mouse pos event
		{
			double MouseX, MouseY;
			EdWindow->GetCursorPos(MouseX, MouseY);

			MouseEvent Ev;
			Ev.Type = EVENT_MOUSE;
			Ev.MouseX = (float)MouseX;
			Ev.MouseY = (float)MouseY;
			Ev.MouseDeltaX = 0.0f;
			Ev.MouseDeltaY = 0.0f;

			EdLayers.OnEvent(Ev);
		}

	}

	void EditorWindow::OnButtonPressed(int32 Button, bool bPressed)
	{
		
	}

	void EditorWindow::OnKeyPressed(int32 Key, bool bPressed)
	{
		
	}

	void EditorWindow::OnKeyChar(uint32 Codepoint)
	{
		
	}

	void EditorWindow::OnScroll(double ScrollX, double ScrollY)
	{
		
	}

	void EditorWindow::Render()
	{
		EdWindow->Update();

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
	
}
