#include "Widget/UserInterface.h"

namespace Ry
{

	UserInterface::UserInterface(Batch* Bat, const StyleSet* Style)
	{
		this->KeyboardFocus = nullptr;
		this->Style = Style;
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

	void UserInterface::SetStyle(const StyleSet* Style)
	{
		this->Style = Style;
		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			RootWidget->SetStyle(Style);
		}
	}

	/**
	 * Adds a widget to the root-level of this user interface.
	 */
	void UserInterface::AddRoot(Ry::SharedPtr<Ry::Widget> Widget)
	{
		RootWidgets.Add(Widget);

		Widget->RenderStateDirty.AddMemberFunction(this, &UserInterface::RenderStateDirty);

		Widget->SetStyle(Style);
		Widget->SetVisible(true, true);

		Draw();
	}

	void UserInterface::Redraw()
	{
		static Ry::ArrayList<Widget*> Children;
		static Ry::ArrayList<PipelineState> PipelineStates;

		Bat->Clear();

		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			RootWidget->Arrange();
			RootWidget->Draw();

			RootWidget->SetVisible(false, true);
			RootWidget->SetVisible(true, true);
		}

		// Update the dynamic pipeline states of all children

		//PipelineStates.SoftClear();
		//AllChildren.SoftClear();

		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			Children.SoftClear();
			PipelineStates.SoftClear();

			RootWidget->GetAllChildren(Children);
			for (Widget* Child : Children)
			{
				Child->GetPipelineStates(PipelineStates);
			}
			for (const PipelineState& State : PipelineStates)
			{
				Bat->UpdatePipelineState(State);
			}
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
			RootWidget->Draw();
		}

		Bat->Update();
	}

	void UserInterface::RenderStateDirty(Widget* Wid, bool bFullRefresh)
	{
		static Ry::ArrayList<PipelineState> PipelineStates;
		static Ry::ArrayList<Widget*> AllChildren;
		static Ry::ArrayList<Widget*> WidgetChildren;
		AllChildren.SoftClear();
		PipelineStates.SoftClear();

		// Takes care of widget visibility changes and widget swapping elements
		{
			WidgetChildren.SoftClear();

			// Add the widget and its children
			Wid->GetAllChildren(WidgetChildren);
			WidgetChildren.Add(Wid);

			for (Widget* Child : WidgetChildren)
			{
				if (Child == Wid)
				{
					Child->OnHide(Bat);
					if (Child->IsVisible())
						Child->OnShow(Bat);
				}
				else
				{
					if (Child->IsVisible())
					{
						Child->OnShow(Bat);
					}
					else
					{
						Child->OnHide(Bat);
					}
				}
			}
		}


		// Takes care of position changes, element changes, etc.
		if (!bFullRefresh)
		{
			// Correctly places widget
			Wid->Arrange();
			Wid->Draw();
		}

		//PipelineStates.SoftClear();
		//AllChildren.SoftClear();

		// Takes care of scissor changes
		if (bFullRefresh)
		{
			for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
			{
				AllChildren.SoftClear();
				PipelineStates.SoftClear();

				RootWidget->GetAllChildren(AllChildren);

				for (Widget* Child : AllChildren)
				{
					Child->GetPipelineStates(PipelineStates);
				}

				for (const PipelineState& State : PipelineStates)
				{
					Bat->UpdatePipelineState(State);
				}

			}
		}
		else
		{
			Wid->GetPipelineStates(PipelineStates);
			for (const PipelineState& State : PipelineStates)
			{
				Bat->UpdatePipelineState(State);
			}
		}


		if (bFullRefresh)
			Draw(); // Re-arrange everything, will also update batch
		else
			Bat->Update(); // Just update batch, this assumes the dirty-ness doesn't affect the positioning of other widgets

	}

}