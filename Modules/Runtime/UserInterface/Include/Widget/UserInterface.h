#pragma once

#include "Widget/Widget.h"
#include "Batch.h"
#include "Event.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE UserInterface : public Widget
	{
	public:

		UserInterface(Batch* Bat = nullptr, const StyleSet* Style = nullptr);
		~UserInterface() = default;
		UserInterface* operator[](Ry::SharedPtr<Widget>& Root);

		/**
		 * Adds a widget to the root-level of this user interface.
		 */
		void AddRoot(Ry::SharedPtr<Ry::Widget> Widget);
		void Redraw();

		// Override functions
		bool OnEvent(const Event& Ev) override;
		void SetStyle(const Ry::StyleSet* Style) override;
		void Arrange() override;
		void Draw() override;
		void Update() override;

		void Rearrange(Widget* Widget) override;
		void FullRefresh() override;
		void UpdateBatch() override;

	private:

		Ry::ArrayList<PipelineState> PipelineStates;

		bool bNeedsFullRefresh;
		bool bUpdateBatch;
		Ry::OASet<Widget*> NeedsReArrange;

		Ry::ArrayList<Ry::SharedPtr<Ry::Widget>> RootWidgets;
		Ry::Widget* KeyboardFocus;
		const Ry::StyleSet* Style;
	};
}
