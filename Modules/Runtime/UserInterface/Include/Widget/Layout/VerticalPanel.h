#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "UserInterfaceGen.h"

namespace Ry
{

	class USERINTERFACE_MODULE VerticalPanel : public PanelWidget
	{		
	public:

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

			VerticalPanel::Slot& operator[](SharedPtr<Ry::Widget> Child)
			{
				this->Widget = Child;
				return *this;
			}

		};

		WidgetBeginArgsSlot(VerticalPanel)
			WidgetProp(float, Pad)
		WidgetEndArgs()

		void Construct(Args& In);
		static Slot MakeSlot();
		virtual Ry::SharedPtr<PanelWidget::Slot> AppendSlot(Ry::SharedPtr<Ry::Widget> Widget) override;

		/**
		 * Arrange widgets vertically.
		 */
		virtual void Arrange() override;
		SizeType ComputeSize() const override;
		void ClearChildren() override;

	private:

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

	};

}
