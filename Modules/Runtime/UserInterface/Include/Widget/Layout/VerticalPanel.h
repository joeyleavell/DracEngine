#pragma once

#include "Widget/Layout/PanelWidget.h"
#include "VerticalPanel.gen.h"

namespace Ry
{

	class USERINTERFACE_MODULE VerticalPanel : public PanelWidget
	{		
	public:

		GeneratedBody()

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

			VerticalPanel::Slot& operator[](SharedPtr<Ry::Widget> Child)
			{
				this->Widget = Child;
				return *this;
			}

		};

		static Slot MakeSlot();
		Ry::SharedPtr<PanelWidgetSlot> AppendSlot(Ry::SharedPtr<Ry::Widget> Widget) override;

		/**
		 * Arrange widgets vertically.
		 */
		virtual void Arrange() override;
		SizeType ComputeSize() const override;
		void ClearChildren() override;

	private:

		Ry::ArrayList<SharedPtr<Slot>> ChildrenSlots;

	} RefClass();

}
