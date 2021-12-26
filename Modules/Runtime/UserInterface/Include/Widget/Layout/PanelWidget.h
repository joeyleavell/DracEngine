#pragma once

#include "Widget/Widget.h"
#include "Data/ArrayList.h"
#include "UserInterfaceGen.h"
#include "Slot/PanelWidgetSlot.h"

namespace Ry
{

	class USERINTERFACE_MODULE PanelWidget : public Widget
	{
	public:
		PanelWidget() = default;

		virtual void ClearChildren();

		SharedPtr<Widget> FindChildWidgetById(const Ry::String& Id) const override;
		void SetStyle(const Ry::StyleSet* Style) override;
		virtual SharedPtr<PanelWidgetSlot> AppendSlot(SharedPtr<Widget> Widget);
		void OnShow(Ry::Batch* Batch) override;
		void OnHide(Ry::Batch* Batch) override;
		void SetParent(Widget* Parent) override;
//		void GetAllChildren(Ry::ArrayList<Widget*>& OutChildren) override;
		void Draw() override;
		void Update() override;
		PanelWidget& operator+(Ry::SharedPtr<Widget>& Widget);
		void SetVisibleFlag(bool bVisibility, bool bPropagate) override;
		bool OnMouseEvent(const MouseEvent& MouseEv) override;
		bool OnMouseButtonEvent(const MouseButtonEvent& MouseEv) override;
		bool OnMouseClicked(const MouseClickEvent& MouseEv) override;
		bool OnMouseDragged(const MouseDragEvent& MouseEv) override;
		bool OnMouseScroll(const MouseScrollEvent& MouseEv) override;
		bool OnKey(const KeyEvent& KeyEv) override;
		bool OnChar(const CharEvent& CharEv) override;
		bool OnPathDrop(const PathDropEvent& PathDropEv) override;
		void GetPipelineStates(Ry::ArrayList<PipelineState>& OutStates, bool bRecurse) override;
		//RectScissor GetClipSpace(const Widget* ForWidget) const override;

		//SizeType GetSlotSize(const Widget* ForWidget, bool bIncludePadding = false) const override;

		SizeType GetUnscaledSlotSize(const Widget* ForWidget) const override;
		SizeType GetUnscaledOccupiedSize(const Widget* ForWidget) const override;
		SizeType GetScaledOccupiedSize(const Widget* ForWidget) const override;

		// int32 NormalizeWidth(int32 InWidth);
		// int32 NormalizeHeight(int32 InHeight);

	protected:
		Ry::ArrayList<SharedPtr<Widget>> Children;

		void RegisterSlot(SharedPtr<PanelWidgetSlot> Slot);
		bool Contains(Widget* const& Widget) const;
		SharedPtr<PanelWidgetSlot> GetSlotForWidget(Widget* const& Widget) const;

	private:
		Ry::OAHashMap<Widget*, SharedPtr<PanelWidgetSlot>> WidgetSlots;
		Ry::OAHashMap<Widget*, bool> CullData;

	} RefClass();

}
