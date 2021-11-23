#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "VerticalPanel.gen.h"
#include "Slot/VerticalPanelSlot.h"

namespace Ry
{

	class USERINTERFACE_MODULE VerticalPanel : public PanelWidget
	{		
	public:

		GeneratedBody()

		static VerticalPanelSlot MakeSlot();
		Ry::SharedPtr<PanelWidgetSlot> AppendSlot(Ry::SharedPtr<Ry::Widget> Widget) override;

		/**
		 * Arrange widgets vertically.
		 */
		void Arrange() override;
		SizeType ComputeSize() const override;
		void ClearChildren() override;

		SizeType GetScaledSlotSize(const Widget* ForWidget) const override;

	private:

		Ry::ArrayList<SharedPtr<VerticalPanelSlot>> ChildrenSlots;

	} RefClass();

}
