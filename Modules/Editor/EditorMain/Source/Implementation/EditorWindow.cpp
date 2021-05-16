#include "EditorWindow.h"
#include "Core/Globals.h"
#include "Interface/RenderAPI.h"
#include "Interface/RenderCommand.h"
#include "SwapChain.h"
#include "Event.h"
#include "EditorUI.h"
#include "Buttons.h"

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

	void EditorWindow::Update(float Delta)
	{
		// Update the layers
		// todo: use real delta time
		EdLayers.Update(0.0f);

		double MouseX, MouseY;
		EdWindow->GetCursorPos(MouseX, MouseY);

		// Send mouse pos event
		{

			MouseEvent Ev;
			Ev.Type = EVENT_MOUSE;
			Ev.MouseX = (float)MouseX;
			Ev.MouseY = (float)MouseY;
			Ev.MouseDeltaX = 0.0f;
			Ev.MouseDeltaY = 0.0f;

			EdLayers.OnEvent(Ev);
		}

		for(int32 Index = 0; Index < MAX_BUTTONS; Index++)
		{
			MouseEventInfo& Info = ButtonsInfo[Index];
			Info.TimeSincePressed += Delta;

			// Check if click event is still applicable
			if(Info.bIsPressed)
			{
				if (!Info.bDrag)
				{
					// Distance factor
					double DX = MouseX - Info.StartX;
					double DY = MouseY - Info.StartY;
					double Dist = sqrt(DX * DX + DY * DY);

					if (Dist >= DoublePressDist)
					{
						Info.bDrag = true;
						Info.bDoublePressEligable = false; // Drag automatically disqualifies double click
					}
				}
				else
				{
					// Fire drag event
					FireDragEvent(Index, MouseX, MouseY);
				}
			}
			else
			{
				// No longer pressed, means stop firing drag event
				Info.bDrag = false;
			}

			if(Info.bDoublePressEligable)
			{
				// Check if still eligible for double press

				// Timing factor
				if (Info.TimeSincePressed >= DoublePressInterval)
				{
					Info.bDoublePressEligable = false;

					// Fire click event if we aren't pressed
					FireClick(Index, MouseX, MouseY);
				}

			}

		}

	}

	void EditorWindow::OnButtonPressed(int32 Button, bool bPressed)
	{
		MouseEventInfo& EventInfo = ButtonsInfo[Button];

		EventInfo.bIsPressed = bPressed;

		double CurX, CurY;
		EdWindow->GetCursorPos(CurX, CurY);

		// Upon the first press, double click events become eligible
		if(bPressed)
		{
			if(!EventInfo.bDoublePressEligable)
			{

				EventInfo.StartX = (int32)CurX;
				EventInfo.StartY = (int32)CurY;
				EventInfo.TimeSincePressed = 0.0f;
				EventInfo.bDoublePressEligable = true;
			}
			else
			{
				FireDoubleClick(Button, CurX, CurY);
				EventInfo.bDoublePressEligable = false;
			}
		}

		// TODO: Fire raw mouse pressed/released event
		
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

	void EditorWindow::FireClick(int32 Button, double XPos, double YPos)
	{
		// Fire double press event
		MouseClickEvent ClickEvent{};
		ClickEvent.Type = EVENT_MOUSE_CLICK;
		ClickEvent.ButtonID = Button;
		ClickEvent.bDoubleClick = false;

		EdLayers.OnEvent(ClickEvent);
	}

	void EditorWindow::FireDoubleClick(int32 Button, double XPos, double YPos)
	{
		// Fire double press event
		MouseClickEvent DoubleClickEvent{};
		DoubleClickEvent.Type = EVENT_MOUSE_CLICK;
		DoubleClickEvent.ButtonID = Button;
		DoubleClickEvent.bDoubleClick = true;

		EdLayers.OnEvent(DoubleClickEvent);
	}

	void EditorWindow::FireDragEvent(int32 Button, float XPos, float YPos)
	{
		MouseDragEvent DragEvent{};
		DragEvent.Type = EVENT_MOUSE_DRAG;
		DragEvent.ButtonID = Button;
		DragEvent.MouseX = XPos;
		DragEvent.MouseY = YPos;

		EdLayers.OnEvent(DragEvent);
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
