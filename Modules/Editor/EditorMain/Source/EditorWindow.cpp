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

		Ry::Delegate<void, int, bool> ButtonPressedDelegate;
		ButtonPressedDelegate.BindMemberFunction(this, &EditorWindow::OnButtonPressed);
		EdWindow->AddMouseButtonDelegate(ButtonPressedDelegate);

		Ry::Delegate<void, uint32> KeyCharDelegate;
		KeyCharDelegate.BindMemberFunction(this, &EditorWindow::OnKeyChar);
		EdWindow->AddKeyCharDelegate(KeyCharDelegate);

		Ry::Delegate<void, int32, KeyAction, int32> KeyPressDelegate;
		KeyPressDelegate.BindMemberFunction(this, &EditorWindow::OnKeyPressed);
		EdWindow->AddKeyPressDelegate(KeyPressDelegate);

		Ry::Delegate<void, double, double> ScrollDelegate;
		ScrollDelegate.BindMemberFunction(this, &EditorWindow::OnScroll);
		EdWindow->AddScrollDelegate(ScrollDelegate);
	}

	void EditorWindow::Update(float Delta)
	{
		EdWindow->Update();

		// Update the layers
		EdLayers.Update(Delta);

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

		// Handle drag events
		for(int32 Index = 0; Index < MAX_BUTTONS; Index++)
		{
			MouseEventInfo& Info = ButtonsInfo[Index];

			// Check if click event is still applicable
			if(Info.bIsPressed)
			{
				if (!Info.bDrag)
				{
					// Distance factor
					double DX = MouseX - Info.StartX;
					double DY = MouseY - Info.StartY;
					double Dist = sqrt(DX * DX + DY * DY);

					if (Dist >= ClickDistThreshold)
					{
						Info.bDrag = true;
					}
				}

				// Fire drag event
				if (Info.bDrag)
				{
					FireDragEvent(Index, MouseX, MouseY);
				}
			}
			else
			{
				// No longer pressed, means stop firing drag event
				Info.bDrag = false;
			}

		}

	}

	void EditorWindow::OnButtonPressed(int32 Button, bool bPressed)
	{
		MouseEventInfo& EventInfo = ButtonsInfo[Button];

		EventInfo.bIsPressed = bPressed;

		double CurX, CurY;
		EdWindow->GetCursorPos(CurX, CurY);

		// TODO: Fire raw mouse pressed/released event
		FireButtonEvent(Button, CurX, CurY, bPressed);

		// Calc distance
		float Dx = CurX - EventInfo.StartX;
		float Dy = CurY - EventInfo.StartY;
		float Dist = std::sqrt(Dx * Dx + Dy * Dy);

		// Upon the first press, double click events become eligible
		if(bPressed)
		{
			// Record start pos
			EventInfo.StartX = (int32)CurX;
			EventInfo.StartY = (int32)CurY;

			// Reset click count if enough time has passed, or passed distance threshold
			if(Dist >= ClickDistThreshold || EventInfo.ClickTimer.is_ready())
			{
				EventInfo.ClickCount = 0;
			}

			EventInfo.ClickTimer.restart();
		}
		else
		{
			// Distance and time check
			if(Dist <= ClickDistThreshold && !EventInfo.ClickTimer.is_ready())
			{
				EventInfo.ClickCount++;

				if(EventInfo.ClickCount == 1)
				{
					// Fire mouse click, 
					FireClick(Button, CurX, CurY);
				}
				else if(EventInfo.ClickCount == 2)
				{
					FireDoubleClick(Button, CurX, CurY);
				}
				else if (EventInfo.ClickCount == 3) // Triple click
				{
					//FireDoubleClick(Button, CurX, CurY);
					EventInfo.ClickCount = 0;
				}
				else
				{
					// Higher order clicks not supported, reset clicks
					EventInfo.ClickCount = 0;
				}

			}

		}

	}

	void EditorWindow::OnKeyPressed(int32 Key, KeyAction Action, int32 Mods)
	{
		FireKeyEvent(Key, Action, Mods);
	}

	void EditorWindow::OnKeyChar(uint32 Codepoint)
	{
		FireCharEvent(Codepoint);
	}

	void EditorWindow::OnScroll(double ScrollX, double ScrollY)
	{
		FireScrollEvent(ScrollX, ScrollY);
	}

	void EditorWindow::FireClick(int32 Button, double XPos, double YPos)
	{
		// Fire double press event
		MouseClickEvent ClickEvent{};
		ClickEvent.Type = EVENT_MOUSE_CLICK;
		ClickEvent.ButtonID = Button;
		ClickEvent.bDoubleClick = false;
		ClickEvent.MouseX = XPos;
		ClickEvent.MouseY = YPos;

		EdLayers.OnEvent(ClickEvent);
	}

	void EditorWindow::FireDoubleClick(int32 Button, double XPos, double YPos)
	{
		// Fire double press event
		MouseClickEvent DoubleClickEvent{};
		DoubleClickEvent.Type = EVENT_MOUSE_CLICK;
		DoubleClickEvent.ButtonID = Button;
		DoubleClickEvent.bDoubleClick = true;
		DoubleClickEvent.MouseX = XPos;
		DoubleClickEvent.MouseY = YPos;

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

	void EditorWindow::FireButtonEvent(int32 Button, float XPos, float YPos, bool bPressed)
	{
		MouseButtonEvent ButtonEvent{};
		ButtonEvent.Type = EVENT_MOUSE_BUTTON;
		ButtonEvent.ButtonID = Button;
		ButtonEvent.MouseX = XPos;
		ButtonEvent.MouseY = YPos;
		ButtonEvent.bPressed = bPressed;

		EdLayers.OnEvent(ButtonEvent);
	}

	void EditorWindow::FireKeyEvent(int32 KeyCode, KeyAction Action, int32 Mods)
	{
		Ry::KeyEvent Ev;
		Ev.Type = EVENT_KEY;
		Ev.Action = Action;
		Ev.KeyCode = KeyCode;
		Ev.bCtrl = (Mods & MOD_CONTROL) == MOD_CONTROL;
		Ev.bShift = (Mods & MOD_SHIFT) == MOD_SHIFT;
		Ev.bAlt = (Mods & MOD_ALT) == MOD_ALT;

		EdLayers.OnEvent(Ev);
	}

	void EditorWindow::FireCharEvent(int32 Codepoint)
	{
		CharEvent Ev;
		Ev.Type = EVENT_CHAR;
		Ev.Codepoint = Codepoint;

		EdLayers.OnEvent(Ev);
	}

	void EditorWindow::FireScrollEvent(float ScrollX, float ScrollY)
	{
		MouseScrollEvent ScrollEvent{};
		ScrollEvent.Type = EVENT_MOUSE_SCROLL;
		ScrollEvent.ScrollX = ScrollX;
		ScrollEvent.ScrollY = ScrollY;

		EdLayers.OnEvent(ScrollEvent);
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
	
}
