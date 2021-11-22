#pragma once

#include "Widget/Widget.h"
#include <vector>
#include "Data/ArrayList.h"
#include "2D/Batch/Batch.h"
#include "UserInterfaceGen.h"
#include "Slot/PanelWidgetSlot.h"

namespace Ry
{

	class USERINTERFACE_MODULE PanelWidget : public Widget
	{
	public:

		WidgetBeginArgsSlot(PanelWidget, Ry::PanelWidgetSlot)
		WidgetEndArgs()

		PanelWidget();

		void Construct(Args& In);
		SharedPtr<Widget> FindChildWidgetById(const Ry::String& Id) const override;
		void SetStyle(const Ry::StyleSet* Style) override;
		virtual SharedPtr<PanelWidgetSlot> AppendSlot(SharedPtr<Widget> Widget);
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void SetParent(Widget* Parent) override;
		void GetAllChildren(Ry::ArrayList<Widget*>& OutChildren) override;
		void Draw() override;
		PanelWidget& operator+(Ry::SharedPtr<Widget>& Widget);
		void SetVisible(bool bVisibility, bool bPropagate) override;
		bool OnMouseEvent(const MouseEvent& MouseEv) override;
		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;
		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;
		bool OnMouseDragged(const MouseDragEvent& MouseEv) override;
		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override;
		bool OnKey(const KeyEvent& KeyEv) override;
		bool OnChar(const CharEvent& CharEv) override;
		virtual void ClearChildren();

	protected:

		Ry::ArrayList<SharedPtr<Widget>> Children;
	} RefClass();

}
