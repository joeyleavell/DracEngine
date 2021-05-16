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

		UserInterface()
		{
			this->KeyboardFocus = nullptr;
		}
		
		~UserInterface() = default;

		UserInterface* operator[](Widget& Root)
		{
			AddRoot(Root);

			return this;
		}

		bool OnEvent(const Event& Ev)
		{
			for(Widget* Root : RootWidgets)
			{
				if(Root->OnEvent(Ev))
				{
					return true;
				}
			}

			return false;
		}

		void SetTextBatch(Batch* Text)
		{
			this->TextBatch = Text;
		}

		void SetShapeBatch(Batch* Shape)
		{
			this->ShapeBatch = Shape;
		}

		void SetTextureBatch(Batch* Shape)
		{
			this->TextureBatch = Shape;
		}

		/**
		 * Adds a widget to the root-level of this user interface.
		 */
		void AddRoot(Ry::Widget& Widget)
		{
			RootWidgets.Add(&Widget);

			// Set the widget's batches
			Widget.SetShapeBatch(ShapeBatch);
			Widget.SetTextBatch(TextBatch);
			Widget.SetTextureBatch(TextureBatch);

			Widget.SetVisible(true, true);

			Widget.RenderStateDirty.AddMemberFunction(this, &UserInterface::RenderStateDirty);
			
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

		void Draw()
		{
			for (Widget* RootWidget : RootWidgets)
			{
				// Arrange the root widget so it is correctly placed
				RootWidget->Arrange();

				// Create the geometry for the elements
				RootWidget->Draw();
			}

			ShapeBatch->Update();
			TextBatch->Update();
			TextureBatch->Update();
		}

	private:

		void RenderStateDirty()
		{
			Draw();
		}

		Ry::Batch* ShapeBatch;
		Ry::Batch* TextBatch;
		Ry::Batch* TextureBatch;

		Ry::ArrayList<Ry::Widget*> RootWidgets;

		Ry::Widget* KeyboardFocus;
	};
}
