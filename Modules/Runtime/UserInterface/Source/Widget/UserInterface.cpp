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

	bool UserInterface::OnEvent(const Event& Ev)
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

		Widget->SetParent(this);
		Widget->SetVisible(true, true);
		Widget->SetStyle(Style);
		Widget->OnShow(Bat);

		bNeedsFullRefresh = true;
	}

	void UserInterface::Redraw()
	{
		// Free pipeline states
		static Ry::ArrayList<PipelineState> PipelineStates;
		PipelineStates.SoftClear();

		// Clear everything from batch
		Bat->Clear();

		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			// Re-arrange and place widget
			RootWidget->Arrange();
			RootWidget->Draw();

			// Re-add all widgets to batch
			RootWidget->OnShow(Bat);

			// Gather pipeline states
			RootWidget->GetPipelineStates(PipelineStates, true);
		}

		// Update all pipeline states
		for (const PipelineState& State : PipelineStates)
		{
			Bat->UpdatePipelineState(State);
		}

		// Update batch
		Bat->Update();
	}

	void UserInterface::Arrange()
	{
		Widget::Arrange();

		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			// Create the geometry for the elements
			RootWidget->Arrange();
		}
	}

	void UserInterface::Draw()
	{
		Widget::Draw();

		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			// Create the geometry for the elements
			RootWidget->Draw();
		}
	}

	void UserInterface::Update()
	{
		// Fast clear lists for this frame
		PipelineStates.SoftClear();

		// Update all widgets
		for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
		{
			RootWidget->Update();
		}

		if(bNeedsFullRefresh)
		{
			// Update all pipeline states, arrange and draw widgets, update batch
			for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
			{
				RootWidget->GetPipelineStates(PipelineStates);
			}

			for (const PipelineState& State : PipelineStates)
			{
				Bat->UpdatePipelineState(State);
			}

			Arrange();
			Draw();

			bUpdateBatch = true;
			bNeedsFullRefresh = false;
		}

		if (NeedsReArrange.GetSize() > 0)
		{
			Ry::OASetIterator<Widget*> WidItr = NeedsReArrange.CreatePairIterator();
			while (WidItr)
			{
				(*WidItr)->Arrange();
				(*WidItr)->Draw();

				// Update pipeline states too
				(*WidItr)->GetPipelineStates(PipelineStates);
				for (const PipelineState& State : PipelineStates)
				{
					Bat->UpdatePipelineState(State);
				}

				++WidItr;
			}

			// Re-arranging widgets performs an implicit batch update
			bUpdateBatch = true;
		}


		if (bUpdateBatch)
		{
			Bat->Update();
			bUpdateBatch = false;
		}

		NeedsReArrange.Clear();
	}

	void UserInterface::Rearrange(Widget* Widget)
	{
		NeedsReArrange.Insert(Widget);
	}

	void UserInterface::FullRefresh()
	{
		bNeedsFullRefresh = true;
	}

	void UserInterface::UpdateBatch()
	{
		bUpdateBatch = true;
	}

}