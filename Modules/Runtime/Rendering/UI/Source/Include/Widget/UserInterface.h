#pragma once

#include "Widget/Widget.h"
#include "RenderingEngine.h"
#include "RenderingPass.h"
#include "2D/Batch/Batch.h"
#include <vector>
#include "UIGen.h"

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

		/**
		 * Adds a widget to the root-level of this user interface.
		 */
		void AddRoot(Ry::Widget& Widget)
		{
			RootWidgets.Add(&Widget);

			Widget.Show();
			
			ReDraw();
		}

		/**
		 * Renders the entire user interface.
		 */
		void Render()
		{
			// Render the UI to the screen
			Ry::ShapeBatcher->Render();

			Ry::TextPass->BeginPass();
			{
				Ry::TextBatcher->Render();
			}
			Ry::TextPass->EndPass();

		}

	private:

		void ReDraw()
		{
			for(Widget* RootWidget : RootWidgets)
			{
				// Arrange the root widget so it is correctly placed
				RootWidget->Arrange();

				// Create the geometry for the elements
				RootWidget->Draw();
			}

			ShapeBatcher->Update();
			TextBatcher->Update();
		}

		Ry::ArrayList<Ry::Widget*> RootWidgets;

		Ry::Widget* KeyboardFocus;
	};
}
