#pragma once

#include "Widget/Widget.h"
#include "Batch.h"
#include "Event.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE UserInterface
	{
	public:

		UserInterface(Batch* Bat = nullptr, const StyleSet* Style = nullptr);
		~UserInterface() = default;
		UserInterface* operator[](Ry::SharedPtr<Widget>& Root);
		bool OnEvent(const Event& Ev);
		void SetBatch(Batch* Bat);
		void SetStyle(const StyleSet* Style);
		
		/**
		 * Adds a widget to the root-level of this user interface.
		 */
		void AddRoot(Ry::SharedPtr<Ry::Widget> Widget);
		void Redraw();
		void Draw();

		void Update();

	private:

		struct UpdateStruct
		{
			Ry::ArrayList<PipelineState> PipelineStates;
			Ry::ArrayList<Widget*> AllChildren;
			Ry::ArrayList<Widget*> WidgetChildren;
		} UpdateValues;

		void OnFullRefresh();
		void OnReArrange(Widget* Wid);
		void OnRePaint(Widget* Wid);

		bool bNeedsFullRefresh;
		Ry::OASet<Widget*> NeedsReArrange;
		Ry::OASet<Widget*> NeedsRePaint;

		void RenderStateDirty(Widget* Wid, bool bRePaint, bool bNeedsReArrange, bool bFullRefresh);

		Ry::Batch* Bat;
		Ry::ArrayList<Ry::SharedPtr<Ry::Widget>> RootWidgets;
		Ry::Widget* KeyboardFocus;
		const Ry::StyleSet* Style;
	};
}
