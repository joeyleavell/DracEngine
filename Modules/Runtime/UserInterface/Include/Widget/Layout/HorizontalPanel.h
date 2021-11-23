#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "Core/Core.h"
#include "Slot/HorizontalPanelSlot.h"
#include "HorizontalPanel.gen.h"

namespace Ry
{

	class USERINTERFACE_MODULE HorizontalPanel : public PanelWidget
	{
	public:

		GeneratedBody()

		static HorizontalPanelSlot MakeSlot();
		
		/**
		 * Arrange widgets horizontally.
		 */
		void Arrange() override;

		SizeType ComputeSize() const override;

		Ry::SharedPtr<PanelWidgetSlot> AppendSlot(Ry::SharedPtr<Ry::Widget> Widget) override;

		void ClearChildren() override;

		SizeType GetScaledSlotSize(const Widget* ForWidget) const override;

	private:

		Ry::ArrayList<SharedPtr<HorizontalPanelSlot>> ChildrenSlots;

	} RefClass();

}
