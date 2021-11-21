#include "Widget/UserInterface.h"

namespace Ry
{

	UserInterface::UserInterface(Batch* Bat)
	{
		this->KeyboardFocus = nullptr;
		SetBatch(Bat);
	}

	UserInterface* UserInterface::operator[](Ry::SharedPtr<Widget>& Root)
	{
		AddRoot(Root);

		return this;
	}

	bool UserInterface::OnEvent(const Event & Ev)
	{
		for (Ry::SharedPtr<Widget>& Root : RootWidgets)
		{
			if (Root->OnEvent(Ev))
			{
				return true;
			}
		}

		return false;
	}

	void UserInterface::SetBatch(Batch * Bat)
	{
		this->Bat = Bat;
	}

	/**
	 * Adds a widget to the root-level of this user interface.
	 */
	void UserInterface::AddRoot(Ry::SharedPtr<Ry::Widget> Widget)
	{
		RootWidgets.Add(Widget);

		Widget->RenderStateDirty.AddMemberFunction(this, &UserInterface::RenderStateDirty);

		Widget->SetVisible(true, true);

		Draw();
	}

	void UserInterface::Redraw()
	{
		Bat->Clear();

		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			RootWidget->Arrange();
			RootWidget->Draw(nullptr);

			RootWidget->SetVisible(false, true);
			RootWidget->SetVisible(true, true);
		}

		// Update the dynamic pipeline states of all children
		static Ry::ArrayList<Widget*> AllChildren;
		static Ry::ArrayList<PipelineState> PipelineStates;

		PipelineStates.SoftClear();
		AllChildren.SoftClear();

		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			RootWidget->GetAllChildren(AllChildren);
		}
		for (Widget* Child : AllChildren)
		{
			Child->GetPipelineStates(PipelineStates);
		}

		for (const PipelineState& State : PipelineStates)
		{
			Bat->UpdatePipelineState(State);
		}


		Bat->Update();
	}

	void UserInterface::Draw()
	{
		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			// Arrange the root widget so it is correctly placed
			RootWidget->Arrange();

			// Create the geometry for the elements
			RootWidget->Draw(nullptr);
		}

		Bat->Update();
	}

	void UserInterface::RenderStateDirty(Widget * Wid, bool bFullRefresh)
	{
		static Ry::ArrayList<PipelineState> PipelineStates;
		static Ry::ArrayList<Widget*> AllChildren;

		// Takes care of widget visibility changes and widget swapping elements
		Wid->OnHide(Bat);
		if (Wid->IsVisible())
			Wid->OnShow(Bat);

		// Correctly places widget
		Wid->Arrange();

		// Takes care of position changes, element changes, etc.
		Wid->Draw(nullptr);

		PipelineStates.SoftClear();
		AllChildren.SoftClear();

		// Takes care of scissor changes
		if (bFullRefresh)
		{
			for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
			{
				RootWidget->GetAllChildren(AllChildren);
			}
		}
		else
		{
			AllChildren.Add(Wid);
		}

		for (Widget* Child : AllChildren)
		{
			Child->GetPipelineStates(PipelineStates);
		}

		for (const PipelineState& State : PipelineStates)
		{
			Bat->UpdatePipelineState(State);
		}

		if (bFullRefresh)
			Draw(); // Re-arrange everything, will also update batch
		else
			Bat->Update(); // Just update batch

	}

}