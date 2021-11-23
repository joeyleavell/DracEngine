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

		PanelWidget();

		SharedPtr<Widget> FindChildWidgetById(const Ry::String& Id) const override;
		void SetStyle(const Ry::StyleSet* Style) override;
		virtual SharedPtr<PanelWidgetSlot> AppendSlot(SharedPtr<Widget> Widget);
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void SetParent(Widget* Parent) override;
		void GetAllChildren(Ry::ArrayList<Widget*>& OutChildren) override;
		void Draw() override;
		PanelWidget& operator+(Ry::SharedPtr<Widget>& Widget);
		void SetVisibleInternal(bool bVisibility, bool bPropagate) override;
		bool OnMouseEvent(const MouseEvent& MouseEv) override;
		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;
		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;
		bool OnMouseDragged(const MouseDragEvent& MouseEv) override;
		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override;
		bool OnKey(const KeyEvent& KeyEv) override;
		bool OnChar(const CharEvent& CharEv) override;
		virtual void ClearChildren();
		//RectScissor GetClipSpace(const Widget* ForWidget) const override;

		//SizeType GetSlotSize(const Widget* ForWidget, bool bIncludePadding = false) const override;

		SizeType GetUnscaledSlotSize(const Widget* ForWidget) const override;
		SizeType GetUnscaledOccupiedSize(const Widget* ForWidget) const override;
		SizeType GetScaledOccupiedSize(const Widget* ForWidget) const override;

		// int32 NormalizeWidth(int32 InWidth);
		// int32 NormalizeHeight(int32 InHeight);


	protected:
		Ry::ArrayList<SharedPtr<Widget>> Children;
		Ry::OAHashMap<Widget*, SharedPtr<PanelWidgetSlot>> WidgetSlots;

	} RefClass();

}
