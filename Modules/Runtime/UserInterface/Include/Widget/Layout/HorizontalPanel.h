#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "Core/Core.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE HorizontalPanel : public PanelWidget
	{
	public:

		WidgetBeginArgsSlot(HorizontalPanel)
		WidgetEndArgs()

		struct Slot : public PanelWidget::Slot
		{
			Slot() :
			PanelWidget::Slot()
			{
			}

			Slot(SharedPtr<Ry::Widget> Wid) :
			PanelWidget::Slot(Wid)
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

		Ry::SharedPtr<PanelWidget::Slot> AppendSlot(Ry::SharedPtr<Ry::Widget> Widget) override;

		void ClearChildren() override;

	private:

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

	};

}
