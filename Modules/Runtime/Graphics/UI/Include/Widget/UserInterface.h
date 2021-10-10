#pragma once

#include "Widget/Widget.h"
#include "RenderingEngine.h"
#include "RenderingPass.h"
#include "2D/Batch/Batch.h"
#include <vector>
#include "UIGen.h"
#include "Event.h"

namespace Ry
{

	class UI_MODULE UserInterface
	{
	public:

		UserInterface(Batch* Bat = nullptr)
		{
			this->KeyboardFocus = nullptr;
			SetBatch(Bat);
		}
		
		~UserInterface() = default;

		UserInterface* operator[](Ry::SharedPtr<Widget>& Root)
		{
			AddRoot(Root);

			return this;
		}

		bool OnEvent(const Event& Ev)
		{
			for(Ry::SharedPtr<Widget>& Root : RootWidgets)
			{
				if(Root->OnEvent(Ev))
				{
					return true;
				}
			}

			return false;
		}

		void SetBatch(Batch* Bat)
		{
			this->Bat = Bat;
		}

		/**
		 * Adds a widget to the root-level of this user interface.
		 */
		void AddRoot(Ry::SharedPtr<Ry::Widget> Widget)
		{
			RootWidgets.Add(Widget);

			Widget->RenderStateDirty.AddMemberFunction(this, &UserInterface::RenderStateDirty);

			Widget->SetVisible(true, true);
			
			Draw();
		}

		void Redraw()
		{
			Bat->Clear();
			
			for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
			{
				RootWidget->Arrange();
				RootWidget->Draw(nullptr);

				RootWidget->SetVisible(false, true);
				RootWidget->SetVisible(true, true);
			}

			Bat->Update();
		}

		void Draw()
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

	private:

		void RenderStateDirty(Widget* Wid, bool bFullRefresh)
		{
			// Takes care of widget visibility changes and widget swapping elements
			Wid->OnHide(Bat);
			if (Wid->IsVisible())
				Wid->OnShow(Bat);

			// Correctly places widget
			Wid->Arrange();

			// Takes care of position changes, element changes, etc.
			Wid->Draw(nullptr);

			// Takes care of scissor changes
			Bat->UpdatePipelineState(Wid->GetPipelineState());

			if(bFullRefresh)
				Draw(); // Re-arrange everything, will also update batch
			else
				Bat->Update(); // Just update batch

		}

		Ry::Batch* Bat;

		Ry::ArrayList<Ry::SharedPtr<Ry::Widget>> RootWidgets;

		Ry::Widget* KeyboardFocus;
	};
}
