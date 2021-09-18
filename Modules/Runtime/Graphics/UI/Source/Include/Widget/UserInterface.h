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

			// Set the widget's batches
			Widget->SetBatch(Bat);

			Widget->SetVisible(true, true);

			Widget->RenderStateDirty.AddMemberFunction(this, &UserInterface::RenderStateDirty);
			
			Draw();
		}

		/**
		 * Renders the entire user interface.
		 */
		void Render()
		{
			// Render the UI to the screen
			// Ry::ShapeBatcher->Render();
			//
			// Ry::TextPass->BeginPass();
			// {
			// 	Ry::TextBatcher->Render();
			// }
			// Ry::TextPass->EndPass();

		}

		void Redraw()
		{
			Bat->Clear();
			
			for (Ry::SharedPtr<Widget>& RootWidget : RootWidgets)
			{
				RootWidget->Arrange();
				RootWidget->Draw();

				RootWidget->SetVisible(false, true);
				RootWidget->SetVisible(true, true);
			}

			Bat->Update();
			//Bat->Render();
		}

		void Draw()
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

	private:

		void RenderStateDirty()
		{
			Draw();
		}

		Ry::Batch* Bat;

		Ry::ArrayList<Ry::SharedPtr<Ry::Widget>> RootWidgets;

		Ry::Widget* KeyboardFocus;
	};
}
