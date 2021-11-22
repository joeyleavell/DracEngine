#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "Core/Core.h"
#include "HorizontalPanel.gen.h"

namespace Ry
{

	class USERINTERFACE_MODULE HorizontalPanel : public PanelWidget
	{
	public:

		GeneratedBody()

		WidgetBeginArgsSlot(HorizontalPanel, PanelWidgetSlot)
		WidgetEndArgs()

		struct Slot : public PanelWidgetSlot
		{
			Slot() :
			PanelWidgetSlot()
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid) :
			PanelWidgetSlot(Wid)
			{
			}
		};

		void Construct(HorizontalPanel::Args& In);
		static Slot MakeSlot();
		
		/**
		 * Arrange widgets horizontally.
		 */
		void Arrange() override;

		SizeType ComputeSize() const override;

		Ry::SharedPtr<PanelWidgetSlot> AppendSlot(Ry::SharedPtr<Ry::Widget> Widget) override;

		void ClearChildren() override;

	private:

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

	} RefClass();

}
